// The iPad and iPhone tab's install queue (round 31): the games of one
// Install, in order, each with its own state, saved to disk so a quit, a
// crash or a dropped device never leaves a game on "Waiting" with nothing
// behind it. Nothing here touches the window or a device: the hub
// (installer/Devices.swift) gives runQueue the function that installs one
// game, and tests/job_queue_check.swift drives the same loop with a fake.
//
// Rules:
// - one game at a time, in the order picked; every game not started says what
//   it waits for ("Waiting for Portal 2 to finish");
// - a game that fails doesn't stop the others; a failure that comes from the
//   device (gone, locked) pauses the rest, which then wait for the device to
//   come back instead of failing one after another;
// - Retry takes the failed and stopped games again, never the done ones (and
//   each game's copy resumes where it stopped: tools/device_copy.py);
// - quitting marks what was running or waiting as stopped, and a reopened
//   installer shows those with Retry, never "Waiting".
import Foundation

enum JobState: Equatable, Codable {
    case waiting(String)      // not started yet: what it waits for
    case running
    case done(String)
    case failed(String)
    case stopped(String)      // the installer quit, or Stop was pressed

    var canRetry: Bool {
        switch self { case .failed, .stopped: return true; default: return false }
    }
}

struct Job: Equatable, Codable {
    var state = JobState.waiting("Next up")
    var fraction = 0.0
    var text = ""
    var deviceProblem = false   // failed because of the device: carries on by itself when it's back
}

/// What installing one game gave: nil problem when it worked.
struct InstallOutcome: Equatable {
    var problem: String?
    var deviceProblem = false
    static let ok = InstallOutcome(problem: nil)
}

struct InstallQueue: Equatable, Codable {
    var device: String           // UDID
    var deviceName: String       // "iPhone", "iPad" (for the sentences)
    var order: [String]          // game ids, in install order
    var titles: [String: String]
    var jobs: [String: Job]
    var started: Date

    static let quitReason = "Stopped when the installer quit. Press Retry to carry on from here."

    init(device: String, deviceName: String, games: [(id: String, title: String)], started: Date = Date()) {
        self.device = device
        self.deviceName = deviceName
        order = games.map(\.id)
        titles = Dictionary(games.map { ($0.id, $0.title) }, uniquingKeysWith: { a, _ in a })
        jobs = Dictionary(games.map { ($0.id, Job()) }, uniquingKeysWith: { a, _ in a })
        self.started = started
        explainWaits()
    }

    var next: String? { order.first { if case .waiting = jobs[$0]?.state { return true }; return false } }
    var running: String? { order.first { jobs[$0]?.state == .running } }
    var finished: Bool { order.allSatisfy { if case .done = jobs[$0]?.state { return true }; return false } }
    var hasRetry: Bool { order.contains { jobs[$0]?.state.canRetry == true } }
    var pausedForDevice: Bool { order.contains { jobs[$0]?.deviceProblem == true && jobs[$0]?.state.canRetry == true } }

    /// Every game not started says which one goes before it.
    mutating func explainWaits() {
        var before: String?
        for id in order {
            guard let job = jobs[id] else { continue }
            if case .waiting(let why) = job.state, !why.hasPrefix("Waiting for the \(deviceName)") {
                jobs[id]?.state = .waiting(before.map { "Waiting for \($0) to finish" } ?? "Next up")
            }
            switch job.state {
            case .waiting, .running: before = titles[id] ?? id
            default: break
            }
        }
    }

    mutating func begin(_ id: String) {
        jobs[id]?.state = .running
        jobs[id]?.deviceProblem = false
        jobs[id]?.text = "Starting"
        explainWaits()
    }

    mutating func finish(_ id: String, _ outcome: InstallOutcome, doneText: String) {
        if let problem = outcome.problem {
            jobs[id]?.state = .failed(problem)
            jobs[id]?.deviceProblem = outcome.deviceProblem
            if outcome.deviceProblem {   // the rest waits for the device instead of failing in turn
                for other in order where other != id {
                    if case .waiting = jobs[other]?.state {
                        jobs[other]?.state = .waiting("Waiting for the \(deviceName) to come back: it carries on by itself")
                    }
                }
            }
        } else {
            jobs[id]?.state = .done(doneText)
            jobs[id]?.fraction = 1
            jobs[id]?.text = ""
        }
        explainWaits()
    }

    /// Retry: the failed and stopped games again, from the first of them; done games stay done.
    mutating func retry() {
        for id in order {
            switch jobs[id]?.state {
            case .failed, .stopped, .waiting: jobs[id]?.state = .waiting("Next up"); jobs[id]?.deviceProblem = false
            default: break
            }
        }
        explainWaits()
    }

    /// Quit or Stop: nothing stays running or waiting.
    mutating func stopAll(_ reason: String = InstallQueue.quitReason) {
        for id in order {
            switch jobs[id]?.state {
            case .running: jobs[id]?.state = .stopped(reason); jobs[id]?.text = ""
            case .waiting: jobs[id]?.state = .stopped("Not started yet: it comes after the one above.")
            default: break
            }
        }
    }

    /// The game whose row carries Retry (Retry carries on the whole queue from there).
    var retryRow: String? { order.first { jobs[$0]?.state.canRetry == true } }

    /// A queue saved by an installer that is gone (quit, crash, Force Quit):
    /// whatever it was doing is stopped, and says so.
    func restored() -> InstallQueue {
        var q = self
        q.stopAll()
        return q
    }

    func save(to url: URL) {
        try? FileManager.default.createDirectory(at: url.deletingLastPathComponent(), withIntermediateDirectories: true)
        if let data = try? JSONEncoder().encode(self) { try? data.write(to: url, options: .atomic) }
    }

    static func load(from url: URL) -> InstallQueue? {
        guard let data = try? Data(contentsOf: url) else { return nil }
        return try? JSONDecoder().decode(InstallQueue.self, from: data)
    }
}

/// The loop: the next waiting game, installed, its outcome noted, saved after
/// every change; a device problem pauses the rest. `stop()` is asked between
/// games (Stop, quit). Returns when nothing is left to start.
@MainActor
func runQueue(_ get: () -> InstallQueue?, _ set: (InstallQueue) -> Void,
              doneText: String, stop: () -> Bool,
              install: (String) async -> InstallOutcome) async {
    while !stop(), var q = get(), let id = q.next {
        if q.pausedForDevice, q.running == nil, case .waiting(let why) = q.jobs[id]?.state,
           why.hasPrefix("Waiting for the \(q.deviceName)") {
            return   // the device comes back first (the hub resumes then)
        }
        q.begin(id)
        set(q)
        let outcome = await install(id)
        guard var after = get() else { return }
        if after.jobs[id]?.state != .running { return }   // stopped meanwhile (quit)
        after.finish(id, outcome, doneText: doneText)
        set(after)
    }
}

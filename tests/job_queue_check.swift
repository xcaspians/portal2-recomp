// installer/JobQueue.swift, driven the way the window drives it (runQueue), with
// a fake install instead of a device: a mid-run failure, a device that drops,
// Retry, a quit in the middle of a game, and the installer reopened after a
// quit or a Force Quit. Built and run by tests/test_job_queue.py:
//   swiftc -parse-as-library installer/JobQueue.swift tests/job_queue_check.swift
import Foundation

var failures = 0
func expect(_ ok: Bool, _ what: String, line: Int = #line) {
    if !ok { failures += 1; print("FAIL (line \(line)): \(what)") }
}
func isWaiting(_ s: JobState?) -> Bool { if case .waiting = s { return true }; return false }
func isDone(_ s: JobState?) -> Bool { if case .done = s { return true }; return false }
func message(_ s: JobState?) -> String {
    switch s {
    case .waiting(let m), .done(let m), .failed(let m), .stopped(let m): return m
    case .running: return "running"
    case nil: return "nil"
    }
}

let games: [(id: String, title: String)] = [("portal2", "Portal 2"), ("hl2", "Half-Life 2"), ("l4d2", "Left 4 Dead 2")]

@MainActor
final class Harness {
    var queue: InstallQueue?
    var installed: [String] = []
    var stop = false
    let file: URL

    init(_ name: String) {
        file = FileManager.default.temporaryDirectory.appendingPathComponent("queue-\(name)-\(getpid()).json")
        try? FileManager.default.removeItem(at: file)
    }

    func run(_ fake: @escaping (String) async -> InstallOutcome) async {
        await runQueue({ self.queue }, { self.queue = $0; $0.save(to: self.file) }, doneText: "Done.", stop: { self.stop }) { id in
            self.installed.append(id)
            return await fake(id)
        }
        queue?.save(to: file)
    }
}

@main
struct JobQueueCheck {
    @MainActor
    static func main() async {
        // Every game not started says what it waits for.
        let fresh = InstallQueue(device: "UDID", deviceName: "iPhone", games: games)
        expect(message(fresh.jobs["portal2"]?.state) == "Next up", "first game: Next up")
        expect(message(fresh.jobs["hl2"]?.state) == "Waiting for Portal 2 to finish", "second waits for the first")
        expect(message(fresh.jobs["l4d2"]?.state) == "Waiting for Half-Life 2 to finish", "third waits for the second")

        // 1. A failure in the middle doesn't wedge the others; Retry takes the failed game only.
        do {
            let h = Harness("failure")
            h.queue = InstallQueue(device: "UDID", deviceName: "iPhone", games: games)
            var hl2Fails = true
            await h.run { id in id == "hl2" && hl2Fails ? InstallOutcome(problem: "setup.sh stopped") : .ok }
            expect(h.installed == ["portal2", "hl2", "l4d2"], "all three tried: \(h.installed)")
            expect(isDone(h.queue?.jobs["portal2"]?.state) && isDone(h.queue?.jobs["l4d2"]?.state), "the others finished")
            expect(h.queue?.jobs["hl2"]?.state == .failed("setup.sh stopped"), "the failure is shown")
            expect(h.queue?.retryRow == "hl2", "Retry sits on the failed game")
            hl2Fails = false
            h.installed = []
            h.queue?.retry()
            await h.run { _ in .ok }
            expect(h.installed == ["hl2"], "Retry picks up from the failed game, not from scratch: \(h.installed)")
            expect(h.queue?.finished == true, "all done after Retry")
        }

        // 2. The device drops during the second game: the third waits for it instead of failing too.
        do {
            let h = Harness("drop")
            h.queue = InstallQueue(device: "UDID", deviceName: "iPhone", games: games)
            await h.run { id in id == "hl2" ? InstallOutcome(problem: "The iPhone didn't come back.", deviceProblem: true) : .ok }
            expect(h.installed == ["portal2", "hl2"], "nothing after the drop: \(h.installed)")
            expect(message(h.queue?.jobs["l4d2"]?.state).hasPrefix("Waiting for the iPhone to come back"),
                   "the third says it waits for the device: \(message(h.queue?.jobs["l4d2"]?.state))")
            expect(h.queue?.pausedForDevice == true, "paused for the device (the hub resumes when it's back)")
            h.installed = []
            h.queue?.retry()   // the device is back
            await h.run { _ in .ok }
            expect(h.installed == ["hl2", "l4d2"], "resumed from the dropped game: \(h.installed)")
            expect(h.queue?.finished == true, "all done")
        }

        // 3. Quit in the middle of the second game, then the installer reopened.
        do {
            let h = Harness("quit")
            h.queue = InstallQueue(device: "UDID", deviceName: "iPhone", games: games)
            await h.run { id in
                if id == "hl2" {   // Cmd+Q, Stop and Quit: the hub's stopForQuit, while the copy runs
                    h.stop = true
                    h.queue?.stopAll()
                    h.queue?.save(to: h.file)
                    return InstallOutcome(problem: "killed")   // what a killed step returns
                }
                return .ok
            }
            expect(h.installed == ["portal2", "hl2"], "nothing started after the quit: \(h.installed)")
            guard let reopened = InstallQueue.load(from: h.file).map({ $0.restored() }) else {
                expect(false, "the queue was saved"); return
            }
            for id in reopened.order {
                expect(!isWaiting(reopened.jobs[id]?.state) && reopened.jobs[id]?.state != .running,
                       "reopened: \(id) isn't Waiting or running: \(message(reopened.jobs[id]?.state))")
            }
            expect(isDone(reopened.jobs["portal2"]?.state), "the done game stays done")
            expect(reopened.jobs["hl2"]?.state == .stopped(InstallQueue.quitReason), "says it stopped at the quit")
            expect(reopened.jobs["l4d2"]?.state.canRetry == true, "the third can go on too")
            expect(reopened.retryRow == "hl2", "Retry on the stopped game")
            let again = Harness("quit-again")
            again.queue = reopened
            again.queue?.retry()
            await again.run { _ in .ok }
            expect(again.installed == ["hl2", "l4d2"], "Retry after reopening carries on: \(again.installed)")
            expect(again.queue?.finished == true, "all done after reopening")
        }

        // 4. Force Quit: the file says a game was running (saved when it began); nothing cleaned up.
        do {
            var q = InstallQueue(device: "UDID", deviceName: "iPhone", games: games)
            q.begin("portal2")
            let file = FileManager.default.temporaryDirectory.appendingPathComponent("queue-force-\(getpid()).json")
            q.save(to: file)
            guard let r = InstallQueue.load(from: file)?.restored() else { expect(false, "saved"); return }
            expect(r.next == nil && r.running == nil, "nothing waits or runs after a Force Quit")
            expect(r.jobs["portal2"]?.state == .stopped(InstallQueue.quitReason), "the running game says it stopped")
            try? FileManager.default.removeItem(at: file)
        }

        if failures == 0 { print("job-queue-check: PASS") } else { print("job-queue-check: \(failures) failure(s)"); exit(1) }
    }
}

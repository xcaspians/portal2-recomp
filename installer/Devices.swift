// The iPad and iPhone side of the Natif Installer (round 28): the Sauce games
// found in Steam, the device plugged in, one Install button, a progress bar
// per game, and the two ways to get the Sauce app itself (TestFlight, or built
// on this Mac and signed with the user's own Apple ID through Xcode).
// tools/devices.py plans, tools/ios_device.sh does the work, setup.sh --ios
// builds; this file only shows and sequences them.
import AppKit
import SwiftUI
import UserNotifications

// MARK: - Games in Steam (same table as tools/steam_library.py GAMES)

struct SauceGame: Identifiable, Equatable {
    let id: String             // tools/devices.py download id
    let title: String
    let appID: Int
    let linuxDepot: Int
    let dataDepot: Int
    let folder: String         // steamapps/common/<folder>
    let marker: String         // a file only a complete install has
    let deviceBytes: Int64     // about what lands on the device (tools/devices.py SIZES and plan labels)
    let setupGame: String?     // setup.sh --game; nil: the installer can't build it yet
    var needs: String? = nil   // another game it runs on, on the device
}

let sauceGames = [
    SauceGame(id: "portal2", title: "Portal 2", appID: 620, linuxDepot: 661, dataDepot: 621, folder: "Portal 2",
              marker: "portal2/pak01_dir.vpk", deviceBytes: 11_500_000_000, setupGame: "portal2"),
    SauceGame(id: "mel", title: "Portal Stories: Mel", appID: 317400, linuxDepot: 317404, dataDepot: 317401,
              folder: "Portal Stories Mel", marker: "portal_stories/bin/server.so", deviceBytes: 6_800_000_000,
              setupGame: "mel", needs: "portal2"),
    SauceGame(id: "portal1", title: "Portal", appID: 400, linuxDepot: 404, dataDepot: 401, folder: "Portal",
              marker: "portal/gameinfo.txt", deviceBytes: 4_400_000_000, setupGame: "portal1"),
    SauceGame(id: "hl2", title: "Half-Life 2", appID: 220, linuxDepot: 224, dataDepot: 221, folder: "Half-Life 2",
              marker: "hl2_complete/gameinfo.txt", deviceBytes: 10_000_000_000, setupGame: "hl2"),
    SauceGame(id: "l4d2", title: "Left 4 Dead 2", appID: 550, linuxDepot: 554, dataDepot: 551, folder: "Left 4 Dead 2",
              marker: "left4dead2/pak01_dir.vpk", deviceBytes: 14_000_000_000, setupGame: nil),
]
let hl2Extras = [("episodic", "Episode One"), ("ep2", "Episode Two"), ("lostcoast", "Lost Coast")]

struct LibraryEntry: Equatable {
    var folder: URL?
    var linux: URL?
    var extras: [String] = []
    var installed: Bool { folder != nil }
    var ready: Bool { folder != nil && linux != nil }
}

/// Every Steam library (libraryfolders.vdf: the default one and any other disk). Reads only what Steam wrote.
func steamLibraries() -> [URL] {
    var out = [steamSupport]
    for vdf in ["steamapps/libraryfolders.vdf", "config/libraryfolders.vdf"] {
        guard let text = try? String(contentsOf: steamSupport.appendingPathComponent(vdf), encoding: .utf8) else { continue }
        for line in text.split(separator: "\n") {
            let parts = line.split(separator: "\"", omittingEmptySubsequences: false)
            // "path"		"/Volumes/Games/SteamLibrary"
            if parts.count >= 4, parts[1] == "path" {
                let url = URL(fileURLWithPath: String(parts[3]).replacingOccurrences(of: "\\\\", with: "\\"))
                if !out.contains(url) { out.append(url) }
            }
        }
    }
    return out
}

func manifestValue(_ text: String, _ key: String) -> String? {
    for line in text.split(separator: "\n") {
        let parts = line.split(separator: "\"", omittingEmptySubsequences: false)
        if parts.count >= 4, parts[1] == key { return String(parts[3]) }
    }
    return nil
}

func scanLibrary(_ game: SauceGame, libraries: [URL]) -> LibraryEntry {
    var entry = LibraryEntry()
    for lib in libraries {
        let manifest = try? String(contentsOf: lib.appendingPathComponent("steamapps/appmanifest_\(game.appID).acf"), encoding: .utf8)
        let dir = manifest.flatMap { manifestValue($0, "installdir") } ?? game.folder
        let candidate = lib.appendingPathComponent("steamapps/common/\(dir)")
        if FileManager.default.fileExists(atPath: candidate.appendingPathComponent(game.marker).path) {
            entry.folder = candidate
            break
        }
    }
    if entry.folder == nil, let depot = findDepot(game.appID, game.dataDepot),
       FileManager.default.fileExists(atPath: depot.appendingPathComponent(game.marker).path) {
        entry.folder = depot
    }
    entry.linux = findDepot(game.appID, game.linuxDepot)
    if game.id == "hl2", let folder = entry.folder {
        entry.extras = hl2Extras.filter { isDir(folder.appendingPathComponent($0.0)) }.map { $0.1 }
    }
    return entry
}

func gigabytes(_ bytes: Int64) -> String { String(format: "%.1f GB", Double(bytes) / 1e9) }

/// Apple silicon, even when the installer runs under Rosetta.
let isAppleSilicon: Bool = {
    var value: Int32 = 0
    var size = MemoryLayout<Int32>.size
    return sysctlbyname("hw.optional.arm64", &value, &size, nil, 0) == 0 && value == 1
}()

// MARK: - Devices (tools/devices.py)

struct DeviceInfo: Decodable, Identifiable, Equatable {
    let udid: String
    let name: String?
    let model: String?
    let kind: String?
    let link: String
    let ready: Bool
    let problem: String?
    var id: String { udid }
    var kindName: String { kind ?? "device" }
}
struct DeviceStatus: Decodable, Equatable {
    var app_installed: Bool?
    var free_bytes: Int64?
    var data: [String: Bool]?
    var apps: [String: Bool]?
    var updates: [String: [String]?]?
    var problem: String?
    func hasUpdate(_ game: String) -> Bool { (updates?[game] ?? nil).map { !$0.isEmpty } ?? false }
}
struct PlanStep: Decodable { let label: String; let command: [String] }
struct DownloadPlan: Decodable { let steps: [PlanStep]; let need_bytes: Int64; let problem: String? }

// tools/devices.py DOWNLOADS: every game, all in the one app (round 19).
let downloads: [(id: String, title: String)] = [("portal2", "Portal 2"), ("mel", "Portal Stories: Mel"),
                                                ("portal1", "Portal"), ("hl2", "Half-Life 2"),
                                                ("l4d2", "Left 4 Dead 2"), ("workshop", "Workshop maps")]
// The one app every game lives in on the device (tools/games.py APP_NAME, round 19).
let deviceAppName = "Sauce"

// The checkout that built the iPhone apps: this installer's own copy, or a
// git checkout named by P2_REPO.
let repoDir = ProcessInfo.processInfo.environment["P2_REPO"].map { URL(fileURLWithPath: $0) } ?? sourceDir
let backupRoot = home.appendingPathComponent("Library/Application Support/Natif Installer/Save backups")

/// A tool's last line as a sentence for the window (tools/devices.py plain_error, plus setup.sh's words).
func plainError(_ text: String, kind: String) -> String {
    let t = text.lowercased()
    if t.contains("locked") || t.contains("passcode") { return "The \(kind) is locked: unlock it and press Retry." }
    if t.contains("trust") || t.contains("pair") { return "The \(kind) doesn't trust this Mac yet: plug it in, unlock it, tap Trust, then press Retry." }
    if t.contains("developer mode") { return "Developer Mode is off on the \(kind): Settings > Privacy & Security > Developer Mode, restart it, then press Retry." }
    if t.contains("not enough space") || t.contains("no space") { return text }
    if t.contains("timed out") || t.contains("not connected") || t.contains("unavailable") || t.contains("stopped answering")
        || t.contains("no longer valid") || t.contains("connection") {
        return "The \(kind) stopped answering: plug it in or bring it near the Mac, unlocked, then press Retry."
    }
    if t.contains("apple development certificate") || t.contains("could not create a profile") {
        return "Xcode couldn't sign Sauce with your Apple ID. Open Xcode > Settings > Accounts, check you're signed in, then press Retry."
    }
    if text.isEmpty { return "Stopped without a message. Press Retry, or open the details below." }
    return "\(text) (press Retry, or open the details below)"
}

enum JobState: Equatable { case waiting, running, done(String), failed(String) }
struct Job: Equatable { var state = JobState.waiting; var fraction = 0.0; var text = "" }

@MainActor
final class DeviceHub: ObservableObject {
    @Published var library: [String: LibraryEntry] = [:]
    @Published var scanned = false
    @Published var list: [DeviceInfo] = []
    @Published var selected: String?
    @Published var status: [String: DeviceStatus] = [:]
    @Published var picks: Set<String> = []
    @Published var userPicked = false
    @Published var jobs: [String: Job] = [:]
    @Published var running = false
    @Published var searching = false
    @Published var log: [String] = []
    @Published var hasXcode = false
    @Published var hasSigningAccount = false
    @Published var expiry: Date?
    private var process: Process?
    private var timer: Timer?
    private var unpacked = false

    var device: DeviceInfo? { list.first { $0.udid == selected } }
    var deviceStatus: DeviceStatus? { selected.flatMap { status[$0] } }
    var sauceOnDevice: Bool { deviceStatus?.app_installed == true }
    var canBuildSauce: Bool { hasXcode && hasSigningAccount && isAppleSilicon }

    func entry(_ game: SauceGame) -> LibraryEntry { library[game.id] ?? LibraryEntry() }
    func onDevice(_ game: SauceGame) -> Bool { deviceStatus?.data?[game.id] == true }
    func pickable(_ game: SauceGame) -> Bool { entry(game).ready && !running }

    /// What Install would write: games not on the device yet (an update is far smaller).
    var needBytes: Int64 {
        sauceGames.filter { picks.contains($0.id) && !onDevice($0) }.map(\.deviceBytes).reduce(0, +)
    }
    var spaceWarning: String? {
        guard let free = deviceStatus?.free_bytes, needBytes > 0, free < needBytes + 1_000_000_000 else { return nil }
        return "This won't fit: the games need about \(gigabytes(needBytes)) plus 1 GB to spare, and the \(device?.kindName ?? "device") has \(gigabytes(free)) free. Untick a game or free some space (Settings > General > Storage)."
    }

    // MARK: Watching

    func start() {
        rescan()
        Task { await refreshDevices(withStatus: false) }
        timer = Timer.scheduledTimer(withTimeInterval: 6, repeats: true) { [weak self] _ in
            Task { @MainActor in
                guard let self, !self.running else { return }
                if sauceGames.contains(where: { !self.entry($0).ready }) || !self.hasSigningAccount { self.rescan() }
            }
        }
    }

    func rescan() {
        Task.detached {
            let libs = steamLibraries()
            let found = Dictionary(uniqueKeysWithValues: sauceGames.map { ($0.id, scanLibrary($0, libraries: libs)) })
            let xcode = FileManager.default.fileExists(atPath: "/Applications/Xcode.app")
                || run("/usr/bin/xcode-select", ["-p"]).output.contains("Xcode")
            let account = run("/usr/bin/security", ["find-certificate", "-c", "Apple Development"]).status == 0
            let expiry = sauceProfileExpiry()
            await MainActor.run {
                self.library = found
                self.scanned = true
                self.hasXcode = xcode
                self.hasSigningAccount = account
                self.expiry = expiry
                if !self.userPicked {   // everything ready starts ticked, until the user changes it
                    self.picks = Set(sauceGames.filter { found[$0.id]?.ready == true }.map(\.id))
                }
                self.picks = self.picks.filter { found[$0]?.ready == true }
            }
        }
    }

    func toggle(_ game: SauceGame, _ on: Bool) {
        userPicked = true
        if on {
            picks.insert(game.id)
            if let base = game.needs, entry(sauceGames.first { $0.id == base }!).ready { picks.insert(base) }
        } else {
            picks.remove(game.id)
            for other in sauceGames where other.needs == game.id && !onDevice(game) { picks.remove(other.id) }
        }
    }

    private func devicesTool(_ args: [String]) async -> Data {
        await withCheckedContinuation { done in
            DispatchQueue.global().async {
                let p = Process()
                p.executableURL = URL(fileURLWithPath: "/usr/bin/python3")
                p.arguments = [repoDir.appendingPathComponent("tools/devices.py").path] + args
                p.currentDirectoryURL = repoDir
                let pipe = Pipe()
                p.standardOutput = pipe
                p.standardError = FileHandle.nullDevice
                try? p.run()
                let data = pipe.fileHandleForReading.readDataToEndOfFile()
                p.waitUntilExit()
                done.resume(returning: data)
            }
        }
    }

    /// withStatus reads the chosen device's free space and games, which opens
    /// Sauce on it for a second: only on Refresh or when a device is chosen.
    func refreshDevices(withStatus: Bool = true) async {
        guard ensureSource() else { return }
        searching = true
        defer { searching = false }
        list = (try? JSONDecoder().decode([DeviceInfo].self, from: await devicesTool(["list"]))) ?? []
        if selected == nil || !list.contains(where: { $0.udid == selected }) {
            selected = list.first(where: { $0.ready })?.udid ?? list.first?.udid
        }
        if withStatus, let d = device, d.ready { await readStatus(d) }
    }

    func choose(_ udid: String) {
        selected = udid
        if let d = device, d.ready, status[udid] == nil { Task { await readStatus(d) } }
    }

    func readStatus(_ d: DeviceInfo) async {
        status[d.udid] = try? JSONDecoder().decode(DeviceStatus.self,
                                                   from: await devicesTool(["status", "--device", d.udid, "--updates"]))
    }

    /// The tools live in the installer's copy of the source (unpacked once per launch).
    private func ensureSource() -> Bool {
        if unpacked || ProcessInfo.processInfo.environment["P2_REPO"] != nil { return true }
        do { try unpackSource(); unpacked = true; return true } catch {
            log.append("error: could not unpack the source: \(error.localizedDescription)")
            return false
        }
    }

    // MARK: Install

    func install() { perform(games: sauceGames.filter { picks.contains($0.id) }) }
    func retry(_ game: SauceGame) { perform(games: [game]) }

    private func perform(games: [SauceGame]) {
        guard let d = device, !games.isEmpty, !running else { return }
        running = true
        for g in games { jobs[g.id] = Job() }
        Task {
            defer { running = false; process = nil }
            guard ensureSource() else {
                for g in games { jobs[g.id]?.state = .failed("The installer couldn't unpack its own files. Reinstall it from the DMG.") }
                return
            }
            if status[d.udid] == nil { await readStatus(d) }
            let backup = backupRoot.appendingPathComponent("\(d.name ?? d.udid)/\(Self.stamp())")
            for g in games {
                jobs[g.id]?.state = .running
                if let failure = await installOne(g, on: d, backup: backup) {
                    jobs[g.id]?.state = .failed(failure)
                } else {
                    jobs[g.id]?.state = .done("Done. Open \(deviceAppName) on the \(d.kindName).")
                    jobs[g.id]?.fraction = 1
                }
                await readStatus(d)
            }
        }
    }

    static func stamp() -> String {
        let f = DateFormatter(); f.dateFormat = "yyyy-MM-dd HH.mm"
        return f.string(from: Date())
    }

    /// nil when it worked, else the sentence to show.
    private func installOne(_ g: SauceGame, on d: DeviceInfo, backup: URL) async -> String? {
        guard let entry = library[g.id], let folder = entry.folder, let linux = entry.linux else {
            return "\(g.title) isn't complete in Steam yet: \(library[g.id]?.installed == true ? "the Linux files are missing" : "install it in Steam first")."
        }
        // 1. Built into this Mac's Sauce? If not, build it (setup.sh --ios: Xcode signs with the user's account).
        var plan = await planFor(g, on: d, backup: backup)
        if let problem = plan?.problem, problem.contains("isn't built") || problem.contains("isn't in this Mac's") {
            guard let setupGame = g.setupGame else {
                return "\(g.title) can't be built by the installer yet. It works when built by hand (docs/l4d2.md)."
            }
            guard canBuildSauce else { return buildBlocker() }
            jobs[g.id]?.text = "Backing up the saves on the \(d.kindName) first"
            if sauceOnDevice, !(await step(["saves-backup", backup.path], device: d.udid, game: g)) {
                return plainError(lastLine, kind: d.kindName)
            }
            let ok = await setup(setupGame, folder: folder, linux: linux, device: d, game: g)
            if !ok { return plainError(lastError ?? lastLine, kind: d.kindName) }
            await readStatus(d)
            scheduleExpiryReminder()
            plan = await planFor(g, on: d, backup: backup)
        }
        guard let plan else { return "The copy couldn't be planned. Check that the \(d.kindName) is unlocked, then press Retry." }
        if let problem = plan.problem {
            return problem.hasSuffix("is up to date on the device.") ? nil : problem
        }
        for (i, s) in plan.steps.enumerated() {
            jobs[g.id]?.text = "\(s.label) (\(i + 1) of \(plan.steps.count))"
            let ok = await step(s.command, device: d.udid, game: g, share: (Double(i), Double(plan.steps.count)))
            if !ok { return plainError(lastLine, kind: d.kindName) }
        }
        return nil
    }

    private func planFor(_ g: SauceGame, on d: DeviceInfo, backup: URL) async -> DownloadPlan? {
        var args = ["plan", "--device", d.udid, "--game", g.id, "--backup", backup.path]
        if status[d.udid]?.hasUpdate(g.id) == true { args.append("--update") }
        return try? JSONDecoder().decode(DownloadPlan.self, from: await devicesTool(args))
    }

    func buildBlocker() -> String {
        if !isAppleSilicon { return "Building Sauce needs a Mac with Apple silicon. On this Mac, use the TestFlight route." }
        if !hasXcode { return "Building Sauce needs Xcode (free, Mac App Store). Install it, open it once, then press Retry." }
        return "Sign in to Xcode with your Apple ID first (Xcode > Settings > Accounts, then Manage Certificates > + > Apple Development), then press Retry."
    }

    private var lastLine = ""
    private var lastError: String?

    /// One tools/ios_device.sh step; "copy i/n" lines move the game's bar inside its share.
    private func step(_ words: [String], device: String, game: SauceGame, share: (Double, Double) = (0, 1)) async -> Bool {
        await launch(repoDir.appendingPathComponent("tools/ios_device.sh"), words, device: device) { line in
            if line.hasPrefix("copy "), let frac = line.dropFirst(5).split(separator: " ").first?.split(separator: "/"),
               frac.count == 2, let a = Double(frac[0]), let b = Double(frac[1]), b > 0 {
                self.jobs[game.id]?.fraction = (share.0 + (a - 1) / b) / share.1
            }
        }
    }

    /// setup.sh --ios for one game, with the device chosen (it builds, signs, installs, copies).
    private func setup(_ id: String, folder: URL, linux: URL, device d: DeviceInfo, game: SauceGame) async -> Bool {
        var p = 0.0
        let ok = await launch(URL(fileURLWithPath: "/bin/bash"),
                              [sourceDir.appendingPathComponent("setup.sh").path, "--game", id] + (id == "mel" ? [] : ["--ios"])
                                  + [folder.path, linux.path], device: d.udid) { line in
            p = Installer.progress(after: line, from: p)
            self.jobs[game.id]?.fraction = p * 0.9
            if line.hasPrefix("==> ") { self.jobs[game.id]?.text = String(line.dropFirst(4)) }
        }
        // Mel joins Portal 2's app: its Mac build first, then Portal 2's iPhone build takes it in.
        if ok && id == "mel", let p2 = library["portal2"], let f = p2.folder, let l = p2.linux {
            return await setup("portal2", folder: f, linux: l, device: d, game: game)
        }
        return ok
    }

    private func launch(_ tool: URL, _ args: [String], device: String, onLine: @escaping @MainActor (String) -> Void) async -> Bool {
        lastLine = ""; lastError = nil
        return await withCheckedContinuation { done in
            let p = Process()
            p.executableURL = tool
            p.arguments = args
            p.currentDirectoryURL = repoDir
            var env = ProcessInfo.processInfo.environment
            env["P2_DEVICE"] = device   // never a guess: the device the user chose
            env["PATH"] = ([home.appendingPathComponent(".cargo/bin").path, "/opt/homebrew/bin"]
                           + ["/usr/bin", "/bin", "/usr/sbin", "/sbin"]).joined(separator: ":")
            p.environment = env
            let pipe = Pipe()
            p.standardOutput = pipe
            p.standardError = pipe
            let buffer = LineBuffer()
            pipe.fileHandleForReading.readabilityHandler = { h in
                let chunk = h.availableData
                guard !chunk.isEmpty else { h.readabilityHandler = nil; return }
                let lines = buffer.append(chunk)
                Task { @MainActor in
                    for line in lines where !line.isEmpty {
                        self.lastLine = line
                        if line.hasPrefix("error:") { self.lastError = String(line.dropFirst(7)) }
                        self.log.append(line)
                        onLine(line)
                    }
                    if self.log.count > 3000 { self.log.removeFirst(self.log.count - 3000) }
                }
            }
            p.terminationHandler = { p in
                Task { @MainActor in done.resume(returning: p.terminationStatus == 0) }
            }
            process = p
            do { try p.run() } catch { done.resume(returning: false) }
        }
    }

    func cancel() { process?.interrupt() }

    // MARK: Refresh Sauce (a free Apple ID's 7 days)

    func refreshSauce() {
        guard let p2 = sauceGames.first(where: { $0.id == "portal2" }) else { return }
        perform(games: [p2])   // with Sauce built: a new profile, the same build re-signed, reinstalled; data kept
    }

    func scheduleExpiryReminder() {
        guard let expiry = sauceProfileExpiry(), expiry.timeIntervalSinceNow < 8 * 86400 else { return }
        let center = UNUserNotificationCenter.current()
        center.requestAuthorization(options: [.alert, .sound]) { granted, _ in
            guard granted else { return }
            let content = UNMutableNotificationContent()
            content.title = "Sauce stops opening tomorrow"
            content.body = "Open the Natif Installer, plug in your iPad or iPhone and press Refresh Sauce. Your saves stay."
            let when = Calendar.current.dateComponents([.year, .month, .day, .hour, .minute], from: expiry.addingTimeInterval(-86400))
            let request = UNNotificationRequest(identifier: "sauce-expiry", content: content,
                                                trigger: UNCalendarNotificationTrigger(dateMatching: when, repeats: false))
            center.add(request)
        }
    }
}

/// When the profile inside this Mac's Sauce build runs out (a free Apple ID's lasts 7 days).
nonisolated func sauceProfileExpiry() -> Date? {
    let profile = repoDir.appendingPathComponent("build/ios/device/Portal2.app/embedded.mobileprovision")
    guard FileManager.default.fileExists(atPath: profile.path) else { return nil }
    let r = run("/usr/bin/security", ["cms", "-D", "-i", profile.path])
    guard r.status == 0, let data = r.output.data(using: .utf8),
          let plist = try? PropertyListSerialization.propertyList(from: data, format: nil) as? [String: Any] else { return nil }
    return plist["ExpirationDate"] as? Date
}

// MARK: - Newer installer on GitHub Releases

@MainActor
final class InstallerUpdate: ObservableObject {
    @Published var newer: (version: String, url: URL)?
    static let api = URL(string: "https://api.github.com/repos/xcaspians/portal2-recomp/releases/latest")!

    /// "v0.6.0" > "v0.5.2-14-gabc" (a build 14 commits after 0.5.2), never the other way.
    nonisolated static func isNewer(_ tag: String, than current: String) -> Bool {
        // The numbers before any "-N-g<hash>" (git describe: commits after that tag).
        func parts(_ v: String) -> [Int] {
            let s = v.hasPrefix("v") ? String(v.dropFirst()) : v
            let nums = (s.split(separator: "-").first ?? "").split(separator: ".").map { Int($0) ?? 0 }
            return nums + Array(repeating: 0, count: max(0, 3 - nums.count))
        }
        let a = parts(tag), b = parts(current)
        guard current.first?.isNumber == true || current.hasPrefix("v") else { return false }   // a development build
        return a.lexicographicallyPrecedes(b) == false && a != b
    }

    func check() {
        Task.detached {
            guard let (data, _) = try? await URLSession.shared.data(from: Self.api),
                  let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
                  let tag = json["tag_name"] as? String, Self.isNewer(tag, than: installerVersion) else { return }
            let assets = json["assets"] as? [[String: Any]] ?? []
            let dmg = assets.compactMap { $0["browser_download_url"] as? String }.first { $0.hasSuffix(".dmg") }
            guard let link = URL(string: dmg ?? (json["html_url"] as? String ?? "")) else { return }
            await MainActor.run { self.newer = (tag, link) }
        }
    }
}

// MARK: - Views

struct DeviceFlowView: View {
    @ObservedObject var hub: DeviceHub
    @ObservedObject var tools: Installer
    @State private var showLog = false

    var body: some View {
        VStack(alignment: .leading, spacing: 18) {
            if !tools.tools {
                StepRow(number: 0, title: "Apple's command line tools", ok: false) {
                    Text("The installer needs them to talk to your iPad or iPhone (about 1 GB, once).")
                    Button("Install…") { tools.installTools() }
                }
            }
            StepRow(number: 1, title: "Your games in Steam", ok: !hub.picks.isEmpty) { gamesList }
            StepRow(number: 2, title: "Your iPad or iPhone", ok: hub.device?.ready == true && hub.sauceOnDevice) { deviceSection }
            StepRow(number: 3, title: "Install", ok: !hub.jobs.isEmpty && hub.jobs.values.allSatisfy { if case .done = $0.state { return true }; return false }) {
                installSection
            }
        }
    }

    var gamesList: some View {
        VStack(alignment: .leading, spacing: 8) {
            if !hub.scanned { Text("Looking in your Steam library…") }
            ForEach(sauceGames) { g in
                let e = hub.entry(g)
                VStack(alignment: .leading, spacing: 2) {
                    HStack {
                        Toggle(isOn: Binding(get: { hub.picks.contains(g.id) }, set: { hub.toggle(g, $0) })) {
                            Text(g.title).font(.headline).foregroundStyle(e.installed ? .primary : .secondary)
                        }
                        .toggleStyle(.checkbox).disabled(!hub.pickable(g))
                        Spacer()
                        if e.installed { Text("about \(gigabytes(g.deviceBytes)) on the device").font(.caption) }
                        if hub.onDevice(g) {
                            Text(hub.deviceStatus?.hasUpdate(g.id) == true ? "update available" : "on the device")
                                .font(.caption).foregroundStyle(hub.deviceStatus?.hasUpdate(g.id) == true ? .orange : .green)
                        }
                    }
                    Group {
                        if !e.installed {
                            Text("Install it in Steam first.")
                        } else if e.linux == nil {
                            Text("One more download from Steam (\(g.title)'s Linux files): open the Steam console and paste this, then wait for “Depot download complete”:")
                                .fixedSize(horizontal: false, vertical: true)
                            HStack {
                                Command(text: "download_depot \(g.appID) \(g.linuxDepot)", copy: tools.copy)
                                Button("Open Steam console") { tools.openSteamConsole() }.controlSize(.small)
                            }
                        } else if !e.extras.isEmpty {
                            Text("With \(e.extras.joined(separator: ", ")).")
                        } else if let base = g.needs, let b = sauceGames.first(where: { $0.id == base }) {
                            Text("Plays on \(b.title)'s files, so \(b.title) comes along.")
                        }
                    }
                    .font(.caption).padding(.leading, 20)
                }
            }
        }
    }

    @ViewBuilder var deviceSection: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text("Plug it in with a cable and unlock it (or keep it on this Wi-Fi).")
                Spacer()
                Button(hub.searching ? "Looking…" : "Refresh") { Task { await hub.refreshDevices() } }
                    .disabled(hub.searching || hub.running)
            }
            if hub.list.isEmpty && !hub.searching {
                Text("No iPad or iPhone found yet.").foregroundStyle(.orange)
            }
            ForEach(hub.list) { d in
                HStack(alignment: .firstTextBaseline) {
                    Image(systemName: hub.selected == d.udid ? "largecircle.fill.circle" : "circle")
                    VStack(alignment: .leading, spacing: 2) {
                        HStack {
                            Text(d.name ?? "Device").bold().foregroundStyle(.primary)
                            Text("\(d.model ?? "") · \(d.link)")
                            if let free = hub.status[d.udid]?.free_bytes { Text("· \(gigabytes(free)) free") }
                        }
                        if let problem = d.problem ?? hub.status[d.udid]?.problem {
                            Text(problem).foregroundStyle(.orange).fixedSize(horizontal: false, vertical: true)
                        }
                    }
                }
                .contentShape(Rectangle())
                .onTapGesture { if !hub.running { hub.choose(d.udid) } }
            }
            if let d = hub.device, d.ready, let st = hub.deviceStatus, st.problem == nil {
                if st.app_installed == true { sauceInstalled } else { SauceRoutes(hub: hub, device: d) }
            } else if hub.device?.ready == true {
                Text("Reading what's on it…").font(.caption)
            }
        }
    }

    @ViewBuilder var sauceInstalled: some View {
        HStack {
            Text("\(deviceAppName) is on it.").foregroundStyle(.green)
            if let expiry = hub.expiry, expiry.timeIntervalSinceNow < 30 * 86400 {
                let days = max(0, Int(expiry.timeIntervalSinceNow / 86400))
                Text(days <= 1 ? "It stops opening \(days == 0 ? "today" : "tomorrow"): refresh it now."
                     : "It keeps opening for \(days) more days (a free Apple ID's limit).")
                    .foregroundStyle(days <= 2 ? .orange : .secondary)
                Button("Refresh Sauce") { hub.refreshSauce() }.disabled(hub.running || !hub.canBuildSauce)
                    .help("Signs Sauce again for 7 days and reinstalls it. Saves and games stay.")
            }
        }
    }

    @ViewBuilder var installSection: some View {
        VStack(alignment: .leading, spacing: 8) {
            let chosen = sauceGames.filter { hub.picks.contains($0.id) }
            if chosen.isEmpty {
                Text("Tick at least one game above.")
            } else if hub.needBytes > 0 {
                Text(hub.sauceOnDevice ? "About \(gigabytes(hub.needBytes)) to copy to the \(hub.device?.kindName ?? "device") (faster over a cable than Wi-Fi)."
                     : "About \(gigabytes(hub.needBytes)) to copy. The first time takes a while: building Sauce on this Mac (half an hour or more), then the copy (faster over a cable than Wi-Fi).")
                    .fixedSize(horizontal: false, vertical: true)
            }
            if let warning = hub.spaceWarning { Text(warning).foregroundStyle(.red).fixedSize(horizontal: false, vertical: true) }
            ForEach(chosen) { g in
                if let job = hub.jobs[g.id] {
                    VStack(alignment: .leading, spacing: 2) {
                        HStack {
                            Text(g.title).foregroundStyle(.primary)
                            Spacer()
                            if case .failed = job.state { Button("Retry") { hub.retry(g) }.disabled(hub.running) }
                        }
                        ProgressView(value: job.fraction)
                        switch job.state {
                        case .waiting: Text("Waiting").font(.caption)
                        case .running: Text(job.text).font(.caption).lineLimit(1)
                        case .done(let m): Text(m).font(.caption).foregroundStyle(.green)
                        case .failed(let m): Text(m).font(.caption).foregroundStyle(.red).fixedSize(horizontal: false, vertical: true)
                        }
                    }
                }
            }
            HStack {
                Button(showLog ? "Hide details" : "Show details") { showLog.toggle() }.buttonStyle(.link)
                Spacer()
                if hub.running { Button("Stop") { hub.cancel() } }
                Button(installTitle(chosen)) { hub.install() }
                    .keyboardShortcut(.defaultAction).controlSize(.large)
                    .disabled(chosen.isEmpty || hub.running || hub.device?.ready != true || !tools.tools || hub.spaceWarning != nil)
            }
            if showLog {
                ScrollView {
                    Text(hub.log.suffix(300).joined(separator: "\n")).font(.system(size: 11, design: .monospaced))
                        .textSelection(.enabled).frame(maxWidth: .infinity, alignment: .leading)
                }
                .frame(height: 140)
                .background(RoundedRectangle(cornerRadius: 6).fill(Color.secondary.opacity(0.08)))
            }
            Text("Saves on the \(hub.device?.kindName ?? "device") are backed up to this Mac before anything there is replaced (Application Support > Natif Installer > Save backups).")
                .font(.caption).fixedSize(horizontal: false, vertical: true)
        }
    }

    func installTitle(_ chosen: [SauceGame]) -> String {
        let updates = chosen.filter { hub.deviceStatus?.hasUpdate($0.id) == true }.count
        if !chosen.isEmpty && chosen.allSatisfy({ hub.onDevice($0) }) { return updates > 0 ? "Update" : "Install again" }
        return "Install \(chosen.count == 1 ? chosen[0].title : "\(chosen.count) games")"
    }
}

/// Sauce isn't on the device: TestFlight, or built here with the user's own Apple ID.
struct SauceRoutes: View {
    @ObservedObject var hub: DeviceHub
    let device: DeviceInfo

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("\(deviceAppName) isn't on this \(device.kindName) yet. Two ways to get it:").foregroundStyle(.primary)
            Card {
                VStack(alignment: .leading, spacing: 4) {
                    Text("A. From TestFlight (if Caspian invited you)").bold()
                    Text("Install Sauce from TestFlight first: on the \(device.kindName), open the TestFlight invite, install TestFlight from the App Store if asked, tap Install next to Sauce, open it once, then press Refresh above. Not open yet: the installer can't copy games into a TestFlight copy until it is.")
                        .fixedSize(horizontal: false, vertical: true)
                }.frame(maxWidth: .infinity, alignment: .leading)
            }
            Card {
                VStack(alignment: .leading, spacing: 4) {
                    Text("B. Built on this Mac with your Apple ID (a free one works)").bold()
                    Text("Install builds Sauce here and signs it with your account through Xcode; the installer never sees your password. A free Apple ID's copy keeps opening for 7 days, then Refresh Sauce here renews it (saves stay).")
                        .fixedSize(horizontal: false, vertical: true)
                    check(isAppleSilicon, "A Mac with Apple silicon", "this Mac has an Intel processor")
                    check(hub.hasXcode, "Xcode installed", "install Xcode from the Mac App Store and open it once")
                    check(hub.hasSigningAccount, "Signed in to Xcode with your Apple ID",
                          "Xcode > Settings > Accounts > + > Apple ID, then Manage Certificates > + > Apple Development")
                    Text("On the \(device.kindName): Settings > Privacy & Security > Developer Mode on (it restarts). After the first install: Settings > General > VPN & Device Management > trust your Apple ID.")
                        .fixedSize(horizontal: false, vertical: true)
                    HStack {
                        if !hub.hasXcode { Button("Open the App Store") { NSWorkspace.shared.open(URL(string: "macappstore://apps.apple.com/app/xcode/id497799835")!) } }
                        if hub.hasXcode && !hub.hasSigningAccount {
                            Button("Open Xcode") { NSWorkspace.shared.open(URL(fileURLWithPath: "/Applications/Xcode.app")) }
                        }
                    }
                }.frame(maxWidth: .infinity, alignment: .leading)
            }
        }
    }

    struct Card<Content: View>: View {
        @ViewBuilder let content: Content
        var body: some View {
            content.padding(10)
                .background(RoundedRectangle(cornerRadius: 8).fill(Color.secondary.opacity(0.08)))
        }
    }

    func check(_ ok: Bool, _ label: String, _ fix: String) -> some View {
        HStack(alignment: .firstTextBaseline, spacing: 6) {
            Image(systemName: ok ? "checkmark.circle.fill" : "circle").foregroundStyle(ok ? .green : .secondary)
            Text(ok ? label : "\(label): \(fix)").fixedSize(horizontal: false, vertical: true)
        }
    }
}

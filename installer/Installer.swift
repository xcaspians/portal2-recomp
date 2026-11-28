// Natif installer: a window around ./setup.sh for people who do not use
// Terminal. It carries a snapshot of the repository (Resources/source.tar.gz,
// made by tools/build_installer.sh), unpacks it in Application Support, finds
// every game of tools/games.py's table the way setup.sh does, then runs setup.sh once per
// game the user ticked and shows its steps. It never downloads a game: the
// Steam console does, by the user's hand.
//
//   installer --snapshot out.png [state]   renders the window off screen
//   installer --detect                     prints what it finds, then exits
import AppKit
import SwiftUI

// $HOME first (the same folder for a Finder app), so tests can point elsewhere.
let home = ProcessInfo.processInfo.environment["HOME"].map { URL(fileURLWithPath: $0) }
    ?? FileManager.default.homeDirectoryForCurrentUser
// Source and build of both games (~5 GB each), apart from their saves.
// The folder name is the first release's (Portal 2 only), kept so that an
// update rebuilds incrementally; uninstall.sh trashes it on its own yes.
let sourceDir = home.appendingPathComponent("Library/Application Support/Portal 2 Natif Build")
let steamSupport = home.appendingPathComponent("Library/Application Support/Steam")
let logFile = home.appendingPathComponent("Library/Logs/NatifInstaller.log")

// MARK: - Games (tools/games.py, the one table)

/// One game of the shared table: tools/games.py's installer entries, which
/// tools/build_installer.sh writes into Resources/games.json. Both tabs (Mac,
/// and iPad and iPhone), setup.sh, tools/steam_library.py and tools/devices.py
/// read the same table; a new game is one entry there, nothing here.
struct Extra: Decodable, Equatable { let id: String; let title: String; let folder: String }

struct Game: Decodable, Identifiable, Equatable {
    let id: String              // setup.sh --game, tools/devices.py download id
    let title: String
    let appID: Int
    let dataDepot: Int          // depot with the game data, separate from the Linux binaries
    let linuxDepot: Int
    let linuxSize: String
    let folder: String          // steamapps/common/<folder>
    let mod: String             // the mod folder inside the game folder
    let markerFile: String      // file that only a complete install has (Steam can leave an empty marker folder)
    let dataFiles: [String]
    let linuxFiles: [String]    // files of the Linux depot (setup.sh's checks)
    let minutes: Int            // first build on the Mac
    let deviceBytes: Int64      // about what lands on the device
    let appName: String         // the Mac app, in ~/Applications
    let needs: String?          // another game it runs on, on the device (Mel: Portal 2)
    let mapCompiler: String?    // Linux depot file of the Puzzle Maker's map compilers (setup.sh --puzzle-maker)
    let experimental: Bool
    let note: String?
    let extras: [Extra]         // games inside its folder (Half-Life 2's episodes and Lost Coast)

    var installedApp: URL { home.appendingPathComponent("Applications/\(appName).app") }
    var depotCommand: String { "download_depot \(appID) \(linuxDepot)" }
    var dataCommand: String { "download_depot \(appID) \(dataDepot)" }

    func isGameFolder(_ url: URL) -> Bool {
        FileManager.default.fileExists(atPath: url.appendingPathComponent(markerFile).path)
    }
    func dataIssue(_ url: URL) -> String? {
        guard id == "portal2" else { return nil }
        let relative = "portal2_dlc2/scripts/editoritems.txt"
        let file = url.appendingPathComponent(relative)
        let info = try? file.resolvingSymlinksInPath().resourceValues(forKeys: [.isRegularFileKey, .fileSizeKey])
        guard info?.isRegularFile == true, (info?.fileSize ?? 0) > 0,
              FileManager.default.isReadableFile(atPath: file.path), isDir(url.appendingPathComponent("platform")) else {
            return "Incomplete game data: check \(relative) (missing, unreadable or empty) and the platform folder. An empty placeholder does not help. In Steam, verify the integrity of Portal 2's files (Properties > Installed Files), choose a complete game folder, or download the game data below:"
        }
        return nil
    }
    func isLinuxDepot(_ url: URL) -> Bool {
        linuxFiles.allSatisfy { FileManager.default.fileExists(atPath: url.appendingPathComponent($0).path) }
    }
    func hasMapCompiler(_ linux: URL?) -> Bool {
        guard let mapCompiler, let linux else { return false }
        return FileManager.default.fileExists(atPath: linux.appendingPathComponent(mapCompiler).path)
    }
    /// The game in any Steam library (its manifest's installdir, else its usual
    /// folder), else its data depot; a complete copy before an incomplete one.
    func findGame(libraries: [URL] = steamLibraries()) -> URL? {
        var candidates: [URL] = libraries.map { lib in
            let manifest = try? String(contentsOf: lib.appendingPathComponent("steamapps/appmanifest_\(appID).acf"), encoding: .utf8)
            return lib.appendingPathComponent("steamapps/common/\(manifest.flatMap { manifestValue($0, "installdir") } ?? folder)")
        }
        if let depot = findDepot(appID, dataDepot) { candidates.append(depot) }
        candidates = candidates.filter(isGameFolder)
        return candidates.first(where: { dataIssue($0) == nil }) ?? candidates.first
    }
    func findLinux() -> URL? {
        if let depot = findDepot(appID, linuxDepot), isLinuxDepot(depot) { return depot }
        return nil
    }
    /// The extras whose folder is in this copy of the game.
    func extrasIn(_ url: URL?) -> [String] {
        guard let url else { return [] }
        return extras.filter { isDir(url.appendingPathComponent($0.folder)) }.map(\.title)
    }
}

/// The table, from the app's games.json (P2_GAMES_JSON for tests and development runs).
func loadGames() -> [Game] {
    let url = ProcessInfo.processInfo.environment["P2_GAMES_JSON"].map { URL(fileURLWithPath: $0) }
        ?? Bundle.main.url(forResource: "games", withExtension: "json")
    guard let url, let data = try? Data(contentsOf: url),
          let list = try? JSONDecoder().decode([Game].self, from: data), !list.isEmpty else {
        FileHandle.standardError.write(Data("games.json is missing or unreadable: rebuild the app with tools/build_installer.sh\n".utf8))
        exit(1)
    }
    return list
}
let allGames = loadGames()
func gameNamed(_ id: String) -> Game? { allGames.first { $0.id == id } }

/// "A, B and C".
func listed(_ items: [String]) -> String {
    items.count < 2 ? items.joined() : items.dropLast().joined(separator: ", ") + " and " + items.last!
}

let mapCompilerMinutes = 10

// MARK: - Detection (same rules as setup.sh)

func isDir(_ url: URL) -> Bool {
    var dir: ObjCBool = false
    return FileManager.default.fileExists(atPath: url.path, isDirectory: &dir) && dir.boolValue
}

/// Folder written by the Steam console's download_depot (setup.sh's find_depot).
func findDepot(_ app: Int, _ id: Int) -> URL? {
    guard isDir(steamSupport) else { return nil }
    let out = run("/usr/bin/find", [steamSupport.path, "-maxdepth", "9", "-type", "d",
                                    "-path", "*/content/app_\(app)/depot_\(id)"]).output
    return out.split(separator: "\n").first.map { URL(fileURLWithPath: String($0)) }
}

/// Command line tools or Xcode selected, with a clang. Never calls a /usr/bin
/// shim such as clang or python3: without the tools, they open Apple's prompt.
func hasCommandLineTools() -> Bool {
    let r = run("/usr/bin/xcode-select", ["-p"])
    let dir = r.output.trimmingCharacters(in: .whitespacesAndNewlines)
    guard r.status == 0, !dir.isEmpty else { return false }
    return ["usr/bin/clang", "Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"].contains {
        FileManager.default.isExecutableFile(atPath: "\(dir)/\($0)")
    }
}

@discardableResult
func run(_ tool: String, _ args: [String]) -> (status: Int32, output: String) {
    let p = Process()
    p.executableURL = URL(fileURLWithPath: tool)
    p.arguments = args
    let pipe = Pipe()
    p.standardOutput = pipe
    p.standardError = FileHandle.nullDevice
    do { try p.run() } catch { return (-1, "") }
    let data = pipe.fileHandleForReading.readDataToEndOfFile()
    p.waitUntilExit()
    return (p.terminationStatus, String(decoding: data, as: UTF8.self))
}

// MARK: - Model

enum Phase: Equatable { case ready, installing, done, failed(String) }

/// What the installer knows about one game.
struct GameState: Equatable {
    var game: URL?
    var linux: URL?
    var selected = false
    var userChose = false      // the user ticked or unticked it: detection no longer changes it
    var built = false
    var dataIssue: String?
    var ready: Bool { game != nil && linux != nil && dataIssue == nil }
}

@MainActor
final class Installer: ObservableObject {
    @Published var tools = false
    @Published var states: [String: GameState] = Dictionary(uniqueKeysWithValues: allGames.map { ($0.id, GameState()) })
    @Published var phase = Phase.ready
    @Published var step = ""
    @Published var progress = 0.0
    @Published var log: [String] = []
    @Published var warnings: [String] = []
    @Published var showLog = false
    @Published var puzzleMaker = false   // the user's wish; buildsMapCompilers says whether it applies
    private var askedForTools = false    // Apple's installer started once this launch
    private var process: Process?
    private var timer: Timer?
    private var queue: [Game] = []       // games still to build in this run
    private var runCount = 1, runIndex = 0
    private var runHasMapCompilers = false   // the current setup.sh run also builds them

    func state(_ game: Game) -> GameState { states[game.id] ?? GameState() }
    var chosen: [Game] { allGames.filter { state($0).selected && state($0).ready } }
    var canInstall: Bool { tools && !chosen.isEmpty && phase != .installing }
    var built: [Game] { allGames.filter { state($0).built } }
    func buildsMapCompilers(_ game: Game) -> Bool {
        puzzleMaker && state(game).selected && game.hasMapCompiler(state(game).linux)
    }
    var minutes: Int { chosen.map { $0.minutes + (buildsMapCompilers($0) ? mapCompilerMinutes : 0) }.reduce(0, +) }

    func setSelected(_ game: Game, _ on: Bool) {
        states[game.id]?.selected = on
        states[game.id]?.userChose = true
    }

    func setGame(_ game: Game, _ url: URL) { states[game.id]?.game = url; tickIfReady(game) }
    func setLinux(_ game: Game, _ url: URL) { states[game.id]?.linux = url; tickIfReady(game) }

    /// A game becomes ticked when it is found, unless the user unticked it.
    private func tickIfReady(_ game: Game) {
        guard var s = states[game.id] else { return }
        s.dataIssue = s.game.flatMap { game.dataIssue($0) }
        if s.ready && !s.userChose { s.selected = true }
        if !s.ready { s.selected = false }
        states[game.id] = s
    }

    func refresh() {
        Task.detached {
            let t = hasCommandLineTools()
            let libs = steamLibraries()
            let found = allGames.map { ($0, $0.findGame(libraries: libs), $0.findLinux()) }
            await MainActor.run {
                self.tools = t
                // No command line tools: Apple's installer first, by itself, before anything needs them
                // (python3 and git are theirs). The window explains, and carries on when they're in.
                if !t, !self.askedForTools, ProcessInfo.processInfo.environment["P2_NO_TOOLS_PROMPT"] == nil {
                    self.askedForTools = true
                    self.installTools()
                }
                for (game, g, l) in found {
                    let s = self.state(game)
                    if s.game == nil || !game.isGameFolder(s.game!) || game.dataIssue(s.game!) != nil { self.states[game.id]?.game = g }
                    if s.linux == nil || !game.isLinuxDepot(s.linux!) { self.states[game.id]?.linux = l }
                    self.tickIfReady(game)
                }
            }
        }
    }

    /// Polls while something is missing: the tools installer and Steam downloads finish on their own.
    func startWatching() {
        refresh()
        timer = Timer.scheduledTimer(withTimeInterval: 4, repeats: true) { [weak self] _ in
            Task { @MainActor in
                guard let self, self.phase != .installing else { return }
                if !self.tools || allGames.contains(where: { !self.state($0).ready }) { self.refresh() }
            }
        }
    }

    func installTools() { run("/usr/bin/xcode-select", ["--install"]) }

    func openSteamConsole() { NSWorkspace.shared.open(URL(string: "steam://open/console")!) }

    func copy(_ text: String) {
        NSPasteboard.general.clearContents()
        NSPasteboard.general.setString(text, forType: .string)
    }

    func choose(_ title: String, expected: String, valid: @escaping (URL) -> Bool, set: @escaping (URL) -> Void) {
        let panel = NSOpenPanel()
        panel.message = title
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        panel.directoryURL = steamSupport
        guard panel.runModal() == .OK, let url = panel.url else { return }
        if valid(url) { set(url) } else {
            let alert = NSAlert()
            alert.messageText = "This is not the right folder"
            alert.informativeText = "Choose the folder that contains \(expected)."
            alert.runModal()
        }
    }

    func install() {
        let games = chosen
        guard !games.isEmpty else { return }
        phase = .installing
        log = []; warnings = []; step = "Unpacking the source"; progress = 0
        for game in allGames { states[game.id]?.built = false }
        queue = games; runCount = games.count; runIndex = 0
        Task.detached {
            do { try unpackSource() } catch {
                await MainActor.run { self.phase = .failed("Could not unpack the source: \(error.localizedDescription)") }
                return
            }
            await MainActor.run { self.runNext() }
        }
    }

    /// setup.sh once per ticked game, with its folders: the same build/ and
    /// config/local.json, each game in its own build tree.
    private func runNext() {
        guard !queue.isEmpty else { phase = .done; return }
        let game = queue.removeFirst()
        let s = state(game)
        guard let gameDir = s.game, let linux = s.linux else { runNext(); return }
        runSetup(game: game, gameDir: gameDir, linux: linux)
    }

    private func runSetup(game: Game, gameDir: URL, linux: URL) {
        let p = Process()
        runHasMapCompilers = buildsMapCompilers(game)
        // Through tools/installer_runs.py: its own process group, gone with the installer (round 31).
        (p.executableURL, p.arguments) = supervised(URL(fileURLWithPath: "/bin/bash"),
            [sourceDir.appendingPathComponent("setup.sh").path, "--game", game.id]
            + (runHasMapCompilers ? ["--puzzle-maker"] : []) + [gameDir.path, linux.path],
            label: "\(game.title): building for the Mac (setup.sh)", game: game.id)
        p.currentDirectoryURL = sourceDir
        // A Finder app gets a bare PATH: toolEnvironment adds Rust's and Homebrew's
        // (setup.sh adds the installer's own tools, tools/build_tools.py env).
        p.environment = toolEnvironment()
        let pipe = Pipe()
        p.standardOutput = pipe
        p.standardError = pipe
        try? FileManager.default.createDirectory(at: logFile.deletingLastPathComponent(), withIntermediateDirectories: true)
        if runIndex == 0 { FileManager.default.createFile(atPath: logFile.path, contents: nil) }
        let logHandle = try? FileHandle(forWritingTo: logFile)
        _ = try? logHandle?.seekToEnd()
        let buffer = LineBuffer()
        // Everything happens on the handler's serial queue: the last bytes are
        // read and logged before the exit status is looked at (end of file).
        pipe.fileHandleForReading.readabilityHandler = { handle in
            let chunk = handle.availableData
            if !chunk.isEmpty {
                logHandle?.write(chunk)
                let lines = buffer.append(chunk)
                if !lines.isEmpty { Task { @MainActor in self.consume(lines) } }
                return
            }
            handle.readabilityHandler = nil
            try? logHandle?.close()
            let rest = buffer.flush()
            p.waitUntilExit()
            let status = p.terminationStatus
            Task { @MainActor in
                self.consume(rest)
                if status == 0 {
                    self.states[game.id]?.built = true
                    self.runIndex += 1
                    self.runNext()
                } else {
                    self.queue = []
                    let error = self.log.last { $0.hasPrefix("error:") } ?? "setup.sh stopped (exit \(status))."
                    self.phase = .failed("\(game.title): " + error.replacingOccurrences(of: "error: ", with: ""))
                    self.showLog = true
                }
            }
        }
        do { try p.run(); process = p } catch { phase = .failed("Could not start setup.sh: \(error.localizedDescription)") }
    }

    /// Progress from setup.sh's output: its steps, then build_game.py's
    /// "lifted a/b jobs" (10-35 %) and "compiled a/b files" (35-95 %). With the
    /// map compilers, a second build follows the game's: the game then fills
    /// 10-55 % and the compilers 55-95 %, each split the same way.
    static func progress(after line: String, from current: Double, mapCompilers: Bool = false) -> Double {
        func fraction(_ prefix: String) -> Double? {
            guard let r = line.range(of: prefix) else { return nil }
            let parts = line[r.upperBound...].prefix { $0 != " " && $0 != "," }.split(separator: "/")
            guard parts.count == 2, let a = Double(parts[0]), let b = Double(parts[1]), b > 0 else { return nil }
            return min(a / b, 1)
        }
        let first = 0.10, last = 0.95, middle = mapCompilers ? 0.55 : last
        let (start, end) = mapCompilers && current >= middle ? (middle, last) : (first, middle)
        let lifted = start + (end - start) * 0.25 / 0.85
        var value = current
        if line.hasPrefix("==> Recompiling") { value = first }
        else if line.hasPrefix("==> Building the Puzzle Maker") { value = middle }
        else if line.hasPrefix("==> Creating") { value = last }
        else if line.hasPrefix("==> ") { value = min(current + 0.012, first) }
        else if let f = fraction("lifted ") { value = start + (lifted - start) * f }
        else if let f = fraction("compiled ") { value = lifted + (end - lifted) * f }
        return max(current, value)
    }

    private func consume(_ lines: [String]) {
        for line in lines {
            // Each game's run fills its own share of the bar.
            let share = 1.0 / Double(max(runCount, 1)), base = Double(runIndex) * share
            let inRun = min(max((progress - base) / share, 0), 1)
            progress = base + share * Installer.progress(after: line, from: inRun, mapCompilers: runHasMapCompilers)
            if line.hasPrefix("==> ") {
                step = String(line.dropFirst(4))
            } else if line.hasPrefix("warning: ") {
                warnings.append(String(line.dropFirst(9)))
            }
            log.append(line)
        }
        if log.count > 4000 { log.removeFirst(log.count - 4000) }
    }

    /// Stops setup.sh and its python/clang children: SIGINT to bash's process
    /// group when it leads one (Process gives it one on macOS), else to bash,
    /// whose trap (setup.sh) forwards the signal to its children.
    func cancel() {
        guard let p = process, p.isRunning else { return }
        if killpg(p.processIdentifier, SIGINT) != 0 { p.interrupt() }
    }

    func open(_ game: Game) { NSWorkspace.shared.open(game.installedApp) }

    /// Quit: installer_runs.py stops the build (Session.stopEverything); the window says so if it stays.
    func stopForQuit() {
        queue = []
        if phase == .installing { phase = .failed("Stopped when the installer quit. Build again to carry on: it only rebuilds what changed.") }
    }
}

/// Splits setup.sh's output into lines. FileHandle calls its readability
/// handler on one serial queue, so the buffer is never touched concurrently.
final class LineBuffer: @unchecked Sendable {
    private var pending = Data()
    func append(_ chunk: Data) -> [String] {
        pending.append(chunk)
        var lines: [String] = []
        while let nl = pending.firstIndex(of: 0x0a) {
            lines.append(String(decoding: pending[pending.startIndex..<nl], as: UTF8.self))
            pending.removeSubrange(pending.startIndex...nl)
        }
        return lines
    }
    /// The unterminated last line, if any.
    func flush() -> [String] {
        defer { pending = Data() }
        return pending.isEmpty ? [] : [String(decoding: pending, as: UTF8.self)]
    }
}

/// Replaces the source in Application Support by the bundled snapshot. build/,
/// deps/ and config/local.json survive, so a new version rebuilds incrementally.
/// The snapshot is unpacked beside the folder, the old source is renamed
/// (.old) and only removed once the new one is in place with the kept items;
/// a failure puts the old source back. A folder without the installer's
/// marker is never touched.
let ownerMarker = ".portal2-installer"

func installerError(_ message: String) -> NSError {
    NSError(domain: "installer", code: 1, userInfo: [NSLocalizedDescriptionKey: message])
}

/// The release this Mac's source is from (SOURCE_VERSION: tools/build_installer.sh, or Update Sauce).
nonisolated func sourceVersion() -> String? {
    (try? String(contentsOf: sourceDir.appendingPathComponent("SOURCE_VERSION"), encoding: .utf8))?
        .trimmingCharacters(in: .whitespacesAndNewlines)
}

nonisolated func unpackSource() throws {
    guard let archive = Bundle.main.url(forResource: "source", withExtension: "tar.gz") else {
        // A bare binary (tests): the source already there is the one to use.
        if ProcessInfo.processInfo.environment["P2_TEST_SOURCE"] != nil,
           FileManager.default.fileExists(atPath: sourceDir.appendingPathComponent(ownerMarker).path) { return }
        throw installerError("source.tar.gz is missing from the app")
    }
    // Update Sauce brought a newer release's source: this older installer doesn't take it back.
    if let current = sourceVersion(), InstallerUpdate.isNewer(current, than: installerVersion) { return }
    _ = try installSource(from: archive, version: nil, keepPrevious: false)
}

let keptItems = ["build", "deps", "config/local.json"]

/// Moves what survives a source change (generated files and the local configuration) between two sources.
nonisolated func carryItems(_ items: [String], from: URL, to: URL) throws {
    let fm = FileManager.default
    for item in items where fm.fileExists(atPath: from.appendingPathComponent(item).path) {
        let dest = to.appendingPathComponent(item)
        try fm.createDirectory(at: dest.deletingLastPathComponent(), withIntermediateDirectories: true)
        try? fm.removeItem(at: dest)
        try fm.moveItem(at: from.appendingPathComponent(item), to: dest)
    }
}

/// The source in `archive` (strip: leading folders, as in GitHub's tag archives) becomes this Mac's,
/// build/, deps/ and the config carried over. keepPrevious: the old source (without those) is
/// returned instead of removed, for restoreSource.
@discardableResult
nonisolated func installSource(from archive: URL, strip: Int = 0, version: String?, keepPrevious: Bool) throws -> URL? {
    let fm = FileManager.default
    let marker = sourceDir.appendingPathComponent(ownerMarker)
    if fm.fileExists(atPath: sourceDir.path), !fm.fileExists(atPath: marker.path),
       !(try fm.contentsOfDirectory(atPath: sourceDir.path)).isEmpty {
        throw installerError("\(sourceDir.path) exists and was not made by this installer: move it away and try again")
    }
    let staging = sourceDir.deletingLastPathComponent().appendingPathComponent(".\(sourceDir.lastPathComponent).staging")
    try? fm.removeItem(at: staging)
    try fm.createDirectory(at: staging, withIntermediateDirectories: true)
    let r = run("/usr/bin/tar", ["-xzf", archive.path, "-C", staging.path] + (strip > 0 ? ["--strip-components", "\(strip)"] : []))
    guard r.status == 0, fm.fileExists(atPath: staging.appendingPathComponent("setup.sh").path) else {
        try? fm.removeItem(at: staging)
        throw installerError("could not unpack the source (tar exit \(r.status))")
    }
    // tar copies the downloaded app's quarantine onto every file it extracts.
    run("/usr/bin/xattr", ["-dr", "com.apple.quarantine", staging.path])
    fm.createFile(atPath: staging.appendingPathComponent(ownerMarker).path, contents: nil)
    if let version { try? Data((version + "\n").utf8).write(to: staging.appendingPathComponent("SOURCE_VERSION")) }
    let old = sourceDir.deletingLastPathComponent().appendingPathComponent(".\(sourceDir.lastPathComponent).\(keepPrevious ? "previous" : "old")")
    try? fm.removeItem(at: old)
    let hadOld = fm.fileExists(atPath: sourceDir.path)
    if hadOld { try fm.moveItem(at: sourceDir, to: old) }
    do {
        try fm.moveItem(at: staging, to: sourceDir)
        if hadOld { try carryItems(keptItems, from: old, to: sourceDir) }
    } catch {
        // Put the old source back, with whatever had already been carried over.
        if hadOld {
            if fm.fileExists(atPath: sourceDir.path) {
                try? carryItems(keptItems, from: sourceDir, to: old)
                try? fm.removeItem(at: sourceDir)
            }
            try? fm.moveItem(at: old, to: sourceDir)
        }
        try? fm.removeItem(at: staging)
        throw error
    }
    if keepPrevious { return hadOld ? old : nil }
    try? fm.removeItem(at: old)
    return nil
}

/// Update Sauce failed: the previous source comes back, with build/, deps/ and the config.
nonisolated func restoreSource(_ previous: URL) throws {
    let fm = FileManager.default
    guard fm.fileExists(atPath: previous.path) else { return }
    try carryItems(keptItems, from: sourceDir, to: previous)
    try fm.removeItem(at: sourceDir)
    try fm.moveItem(at: previous, to: sourceDir)
}

// The installer's version (tools/build_installer.sh: git describe), shown in the window.
let installerVersion = (Bundle.main.object(forInfoDictionaryKey: "P2SourceVersion") as? String)
    ?? (Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as? String) ?? "development build"

// The device side (games found in Steam, the iPad or iPhone, Install) is installer/Devices.swift.

// MARK: - Views

struct StepRow<Content: View>: View {
    let number: Int
    let title: String
    let ok: Bool
    @ViewBuilder let content: Content

    var body: some View {
        HStack(alignment: .firstTextBaseline, spacing: 12) {
            ZStack {
                Circle().fill(ok ? Color.green : Color.secondary.opacity(0.18)).frame(width: 22, height: 22)
                if ok { Image(systemName: "checkmark").font(.system(size: 11, weight: .bold)).foregroundStyle(.white) }
                else { Text("\(number)").font(.system(size: 12, weight: .semibold)).foregroundStyle(.secondary) }
            }
            .alignmentGuide(.firstTextBaseline) { $0[VerticalAlignment.center] + 5 }
            VStack(alignment: .leading, spacing: 6) {
                Text(title).font(.headline)
                content.font(.callout).foregroundStyle(.secondary)
            }
            Spacer(minLength: 0)
        }
    }
}

struct Command: View {
    let text: String
    let copy: (String) -> Void
    var body: some View {
        HStack(spacing: 6) {
            Text(text).font(.system(.callout, design: .monospaced)).textSelection(.enabled)
                .padding(.horizontal, 8).padding(.vertical, 3)
                .background(RoundedRectangle(cornerRadius: 5).fill(Color.secondary.opacity(0.12)))
            Button("Copy") { copy(text) }.controlSize(.small)
        }
    }
}

struct PathLine: View {
    let url: URL?
    let missing: String
    var body: some View {
        if let url { Text(url.path).lineLimit(1).truncationMode(.middle).help(url.path) }
        else { Text(missing).fixedSize(horizontal: false, vertical: true) }
    }
}

let windowTitle = "Sauce Installer"

/// One game: a checkbox, then where its files are or how to get them.
struct GameRow: View {
    @ObservedObject var model: Installer
    let game: Game

    var body: some View {
        let s = model.state(game)
        VStack(alignment: .leading, spacing: 6) {
            Toggle(isOn: Binding(get: { s.selected }, set: { model.setSelected(game, $0) })) {
                Text(game.title).font(.headline).foregroundStyle(.primary)
            }
            .toggleStyle(.checkbox)
            .disabled(!s.ready || model.phase == .installing)
            VStack(alignment: .leading, spacing: 6) {
                if let url = s.game {
                    PathLine(url: url, missing: "")
                    let extras = game.extrasIn(url)
                    if !extras.isEmpty { Text("With \(listed(extras)): they start from \(game.title)'s own menu.") }
                    if let issue = s.dataIssue {
                        Text(issue).foregroundStyle(.orange).fixedSize(horizontal: false, vertical: true)
                        Command(text: game.dataCommand, copy: model.copy)
                    }
                } else {
                    Text("Game data not found: install \(game.title) with Steam (the current version), choose its folder, or paste this in the Steam console and wait for “Depot download complete”:")
                        .fixedSize(horizontal: false, vertical: true)
                    Command(text: game.dataCommand, copy: model.copy)
                }
                if let url = s.linux {
                    PathLine(url: url, missing: "")
                } else {
                    Text("Linux binaries: paste this in the Steam console, then wait for “Depot download complete” (\(game.linuxSize)):")
                        .fixedSize(horizontal: false, vertical: true)
                    Command(text: game.depotCommand, copy: model.copy)
                }
                if s.ready { Text("Ready. About \(game.minutes) minutes to build the first time.") }
                if let note = game.note { Text(note).font(.caption) }
                HStack {
                    if !s.ready { Button("Open Steam console") { model.openSteamConsole() } }
                    if s.game == nil || s.dataIssue != nil {
                        Button("Choose game folder…") {
                            model.choose("Choose the \(game.title) folder (it contains \(game.mod))", expected: "the \(game.mod) folder",
                                         valid: game.isGameFolder) { model.setGame(game, $0) }
                        }
                    }
                    if s.linux == nil {
                        Button("Choose Linux binaries…") {
                            model.choose("Choose the depot_\(game.linuxDepot) folder of the Linux binaries",
                                         expected: game.linuxFiles[0], valid: game.isLinuxDepot) { model.setLinux(game, $0) }
                        }
                    }
                }
                if game.mapCompiler != nil, s.ready {
                    let available = game.hasMapCompiler(s.linux)
                    Toggle(available ? "Puzzle Maker: also build the map compilers (about \(mapCompilerMinutes) more minutes, 220 MB)"
                                     : "Puzzle Maker: unavailable, no \((game.mapCompiler! as NSString).lastPathComponent) in these Linux binaries",
                           isOn: Binding(get: { model.puzzleMaker && available }, set: { model.puzzleMaker = $0 }))
                        .toggleStyle(.checkbox)
                        .disabled(!available || !s.selected || model.phase == .installing)
                        .help("Needed for Build and Play in Community Test Chambers. Without them the editor opens, but a chamber cannot be built.")
                }
            }
            .padding(.leading, 20)
            .font(.callout).foregroundStyle(.secondary)
        }
    }
}

struct ContentView: View {
    @ObservedObject var model: Installer

    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            Text(isAppleSilicon ? "Builds native Mac apps from your own copies of the games on Steam. Nothing from the games is downloaded by this installer."
                 : "This Mac has an Intel processor: the Mac games need Apple silicon (M1 or newer), so they can't be built here.")
                .font(.callout).foregroundStyle(isAppleSilicon ? Color.secondary : Color.orange).fixedSize(horizontal: false, vertical: true)
                .padding(.bottom, 16)
            VStack(alignment: .leading, spacing: 18) {
                StepRow(number: 1, title: "Xcode command line tools", ok: model.tools) {
                    if model.tools { Text("Installed.") } else {
                        Text("Apple's compiler, needed to build the games (about 1 GB).")
                        Button("Install…") { model.installTools() }
                    }
                }
                StepRow(number: 2, title: "Games to build", ok: !model.chosen.isEmpty) {
                    VStack(alignment: .leading, spacing: 14) {
                        ForEach(allGames) { GameRow(model: model, game: $0) }
                        let experimental = allGames.filter(\.experimental).map(\.title)
                        Text("\(listed(experimental)) are experimental: they play on Mac, with fewer hours of testing.")
                            .font(.caption)
                    }
                }
                StepRow(number: 3, title: "Build", ok: model.phase == .done) {
                    buildStatus
                }
            }

            Spacer(minLength: 16)
            footer
        }
    }

    @ViewBuilder var buildStatus: some View {
        switch model.phase {
        case .ready:
            let games = model.chosen
            Text(games.isEmpty ? "Tick at least one game found above."
                 : "About \(model.minutes) minutes the first time and \(5 * games.count) GB of disk. Re-running later only rebuilds what changed.")
        case .installing:
            ProgressView(value: model.progress)
            Text(model.step).lineLimit(1)
        case .done:
            Text("\(model.built.map { "“\($0.appName)”" }.joined(separator: " and ")) \(model.built.count > 1 ? "are" : "is") in your Applications folder. Open Steam with the account that owns \(model.built.count > 1 ? "the games, then open an app" : "the game, then open the app").")
                .fixedSize(horizontal: false, vertical: true)
            ForEach(model.warnings, id: \.self) { Text("⚠︎ " + $0).foregroundStyle(.orange) }
        case .failed(let message):
            Text(message).foregroundStyle(.red).fixedSize(horizontal: false, vertical: true)
        }
    }

    var footer: some View {
        VStack(alignment: .leading, spacing: 10) {
            if model.showLog {
                ScrollViewReader { proxy in
                    ScrollView {
                        Text(model.log.suffix(400).joined(separator: "\n"))
                            .font(.system(size: 11, design: .monospaced)).textSelection(.enabled)
                            .frame(maxWidth: .infinity, alignment: .leading).id("end")
                    }
                    .frame(height: 140)
                    .background(RoundedRectangle(cornerRadius: 6).fill(Color.secondary.opacity(0.08)))
                    .onChange(of: model.log.count) { _ in proxy.scrollTo("end", anchor: .bottom) }
                }
            }
            HStack {
                Button(model.showLog ? "Hide details" : "Show details") { model.showLog.toggle() }
                    .buttonStyle(.link)
                Spacer()
                switch model.phase {
                case .installing:
                    Button("Stop") { model.cancel() }
                case .done:
                    Button("Rebuild") { model.install() }
                    ForEach(model.built) { game in
                        Button("Open \(game.title)") { model.open(game) }
                    }
                default:
                    Button(model.phase == .ready ? "Build" : "Try again") { model.install() }
                        .keyboardShortcut(.defaultAction).disabled(!model.canInstall)
                }
            }
        }
    }
}

/// The window: the iPad and iPhone side first (round 28), the Mac apps on the second tab.
struct RootView: View {
    @ObservedObject var model: Installer
    @ObservedObject var hub: DeviceHub
    @ObservedObject var update: InstallerUpdate
    @State var tab = 0
    var snapshot = false   // ImageRenderer draws neither a segmented control nor a scroll view

    var page: some View {
        Group {
            if tab == 0 { DeviceFlowView(hub: hub, tools: model) } else { ContentView(model: model) }
        }
        .padding(.vertical, 8).padding(.trailing, 8)
        .frame(maxWidth: .infinity, alignment: .leading)
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            HStack(alignment: .firstTextBaseline) {
                Text("Sauce for iPad, iPhone and Mac").font(.title2.weight(.semibold))
                Spacer()
                Text("Version \(installerVersion)").font(.caption).foregroundStyle(.secondary).textSelection(.enabled)
            }
            Text("Plays your own Steam copies of \(listed(allGames.map(\.title))). Nothing from the games is downloaded by this installer, and it never asks for your Steam password.")
                .font(.callout).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            if let newer = update.newer {
                HStack {
                    Text("Natif Installer \(newer.version) is out.")
                    Button("Download") { NSWorkspace.shared.open(newer.url) }
                }
                .padding(8).background(RoundedRectangle(cornerRadius: 6).fill(Color.accentColor.opacity(0.12)))
            }
            if snapshot {
                Text(tab == 0 ? "[ iPad and iPhone | Mac ]" : "[ iPad and iPhone | Mac ]").font(.caption.monospaced())
                page
            } else {
                Picker("", selection: $tab) {
                    Text("iPad and iPhone").tag(0)
                    Text("Mac").tag(1)
                }
                .pickerStyle(.segmented).labelsHidden()
                ScrollView { page }
            }
        }
        .padding(24)
        .frame(width: 640)
        .frame(minHeight: 680)
    }
}

// MARK: - App

final class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool { true }

    /// Cmd+Q with a build going asks first; quitting stops everything the installer started (round 31:
    /// it used to leave setup.sh, ninja and devicectl copies running with no window).
    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        MainActor.assumeIsolated {
            if let busy = Session.busy {
                let alert = NSAlert()
                alert.messageText = "A build is running. Stop it and quit?"
                alert.informativeText = "\(busy) Quitting stops it. Next time, Retry carries on from where it stopped, and the saves on your device are safe."
                alert.addButton(withTitle: "Stop and Quit")
                alert.addButton(withTitle: "Keep Running")
                guard alert.runModal() == .alertFirstButtonReturn else { return .terminateCancel }
            }
            Session.stopEverything()
            return .terminateNow
        }
    }
}

struct InstallerApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var delegate
    @StateObject private var model = Installer()
    @StateObject private var hub = DeviceHub()
    @StateObject private var update = InstallerUpdate()
    var body: some Scene {
        WindowGroup(windowTitle) {
            RootView(model: model, hub: hub, update: update).onAppear {
                Session.hub = hub; Session.model = model
                model.startWatching(); hub.start(); update.check()
            }
        }
        .windowResizability(.contentSize)
    }
}

@MainActor
func snapshot(to path: String, state: String) {
    let model = Installer()
    model.tools = state != "empty"
    if state != "empty" {
        model.states["portal2"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Portal 2"),
                                            linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_661"), selected: state != "missing")
        model.states["portal1"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Portal"),
                                            linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_404"),
                                            selected: state != "missing")
        model.states["hl2"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Half-Life 2"),
                                        linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_224"), selected: state != "missing")
        model.states["mel"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Portal Stories Mel"),
                                        linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_317404"),
                                        selected: state != "missing")
        model.states["l4d2"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Left 4 Dead 2"),
                                         linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_554"),
                                         selected: state != "missing")
    }
    switch state {
    case "incomplete":
        model.states["portal2"]?.dataIssue = "Incomplete game data: portal2_dlc2/scripts/editoritems.txt is missing, unreadable or empty. An empty placeholder does not help. In Steam, verify the integrity of Portal 2's files (Properties > Installed Files), choose a complete game folder, or download the game data below:"
        model.states["portal2"]?.selected = false
    case "puzzlemaker": model.puzzleMaker = true
    case "building": model.phase = .installing
        model.step = "Recompiling Portal 2 (about 10 minutes the first time, seconds afterwards)"; model.progress = 0.25
        model.showLog = true; model.log = ["==> Preparing the Linux modules of Portal 2", "[lift] engine.so: 41 233 functions", "[cc] 312/1024"]
    case "done": model.phase = .done
        for game in allGames { model.states[game.id]?.built = true }
    case "failed": model.phase = .failed("Half-Life 2: lib/libSDL2.dylib not found")
    default: break
    }
    let hub = DeviceHub()
    if state.hasPrefix("devices") {   // the iPad and iPhone tab, made-up devices and games
        let home = URL(fileURLWithPath: "/Users/you/Library/Application Support/Steam/steamapps/common")
        hub.scanned = true
        hub.library = ["portal2": LibraryEntry(folder: home.appendingPathComponent("Portal 2"), linux: URL(fileURLWithPath: "/depot_661")),
                       "mel": LibraryEntry(folder: home.appendingPathComponent("Portal Stories Mel"), linux: nil),
                       "hl2": LibraryEntry(folder: home.appendingPathComponent("Half-Life 2"), linux: URL(fileURLWithPath: "/depot_224"),
                                           extras: ["Episode One", "Episode Two", "Lost Coast"]),
                       "l4d2": LibraryEntry(folder: home.appendingPathComponent("Left 4 Dead 2"), linux: URL(fileURLWithPath: "/depot_554"))]
        hub.picks = ["portal2", "hl2", "l4d2"]
        hub.hasXcode = true
        hub.list = [DeviceInfo(udid: "A", name: "My iPad Pro", model: "iPad Pro 11-inch (M5)", kind: "iPad", link: "USB", ready: true, problem: nil),
                    DeviceInfo(udid: "B", name: "iPhone", model: "iPhone 18 Pro", kind: "iPhone", link: "Wi-Fi", ready: false,
                               problem: "Not trusted yet: unlock it, plug it in, and tap Trust on its screen.")]
        hub.selected = "A"
        let installed = state != "devices-new"
        hub.status["A"] = DeviceStatus(app_installed: installed, free_bytes: state == "devices-full" ? 12_000_000_000 : 80_000_000_000,
                                       data: installed ? ["portal2": true] : [:], apps: nil,
                                       updates: installed ? ["portal2": ["Documents/prep"]] : nil, problem: nil)
        if state == "devices-running" {
            var q = InstallQueue(device: "A", deviceName: "iPad", games: [("portal2", "Portal 2"), ("hl2", "Half-Life 2"), ("l4d2", "Left 4 Dead 2")])
            q.begin("portal2"); q.finish("portal2", .ok, doneText: "Done. Open Sauce on the iPad.")
            q.begin("hl2"); q.jobs["hl2"]?.fraction = 0.42; q.jobs["hl2"]?.text = "Copy Half-Life 2 (about 10 GB) (2 of 3)"
            hub.queue = q
            hub.running = true
        }
        if state == "devices-failed" {
            var q = InstallQueue(device: "A", deviceName: "iPad", games: [("portal2", "Portal 2")])
            q.begin("portal2"); q.finish("portal2", InstallOutcome(problem: plainError("device is locked", kind: "iPad"), deviceProblem: true), doneText: "")
            hub.queue = q
        }
        hub.expiry = Date().addingTimeInterval(3 * 86400)
        hub.toolsChecked = true
        // Round 31's screens: a copy paused by a lock or a drop, an install waiting on a leftover or
        // on another window, a quit mid-install reopened, and Update Sauce.
        if ["devices-locked", "devices-dropped", "devices-leftover", "devices-other", "devices-reopened"].contains(state) {
            hub.list[0] = DeviceInfo(udid: "A", name: "My iPhone", model: "iPhone 18 Pro", kind: "iPhone", link: "Wi-Fi", ready: true, problem: nil)
            var q = InstallQueue(device: "A", deviceName: "iPhone", games: [("portal2", "Portal 2"), ("hl2", "Half-Life 2"), ("l4d2", "Left 4 Dead 2")])
            q.begin("portal2"); q.finish("portal2", .ok, doneText: "Done. Open Sauce on the iPhone.")
            switch state {
            case "devices-locked", "devices-dropped":
                q.begin("hl2"); q.jobs["hl2"]?.fraction = 0.55
                q.jobs["hl2"]?.text = state == "devices-locked"
                    ? "Your iPhone locked. Unlock it to carry on (with iPhone Mirroring, unlock the iPhone itself). The copy picks up where it stopped."
                    : "The iPhone dropped off the connection. Plug it in with a cable, or bring it near the Mac and unlock it: the copy carries on by itself."
                hub.running = true
            case "devices-leftover":
                q.jobs["hl2"]?.state = .waiting("Waiting for the earlier run's build to stop (see below)")
                hub.leftover = "Waiting for a build left over from an earlier run (Left 4 Dead 2: data on the iPhone, started 4:12 PM). No installer window is attached to it any more, so it would never finish here."
                hub.running = true
            case "devices-other":
                q.jobs["hl2"]?.state = .waiting("Waiting for the last install to finish (see below)")
                hub.activity = "Waiting for the last install to finish (Left 4 Dead 2: data on the iPhone, started 4:12 PM): another Natif Installer window is running it. This one starts by itself when it's done."
                hub.running = true
            default:   // reopened after a quit mid-install
                q.begin("hl2"); q.jobs["hl2"]?.fraction = 0.4
                q = q.restored()
            }
            hub.queue = q
            hub.status["A"]?.sauce_version = "0.5.1"
            hub.sauceLatest = "v0.5.2"
        }
        if state == "devices-update" {
            hub.status["A"]?.sauce_version = "0.5.1"
            hub.sauceLatest = "v0.5.2"
            hub.hasSigningAccount = true
            hub.updateJob = Job(state: .running, fraction: 0.35, text: "Building Sauce v0.5.2 (a while: half an hour or more if a lot changed)")
        }
        if state == "devices-devmode" {   // a fresh iPad: Developer Mode off, Install pressed, waiting
            hub.list = [DeviceInfo(udid: "A", name: "My iPad", model: "iPad Air", kind: "iPad", link: "USB", ready: false,
                                   problem: "Developer Mode is off: turn it on once, then the install carries on by itself.",
                                   fix: "developer-mode",
                                   steps: ["Keep the iPad plugged into this Mac and unlocked. (The Developer Mode switch only appears once the iPad has been paired with a Mac, which is done.)",
                                           "On the iPad, open Settings → Privacy & Security, scroll to the bottom and tap Developer Mode.",
                                           "Turn Developer Mode On, then tap Restart.",
                                           "When the iPad has restarted, unlock it, tap Turn On in the message that appears, and enter the passcode.",
                                           "That’s all: the installer sees it come back and starts the install by itself."])]
            hub.status = [:]
            hub.waitingFor = "A"
        }
        if state == "devices-tools" {   // a fresh Mac: Xcode there, the build tools and Rust not yet
            hub.toolChecks = [
                ToolCheck(id: "xcodegen", title: "XcodeGen", ok: false, problem: "XcodeGen is missing (it is needed for signing Sauce with your Apple ID): Set up tools installs it.", action: "install", size_mb: 4),
                ToolCheck(id: "cmake", title: "CMake", ok: false, problem: "CMake is missing (it is needed for building SDL2, FreeType and gl4es for Sauce): Set up tools installs it.", action: "install", size_mb: 89),
                ToolCheck(id: "rust", title: "Rust", ok: false, problem: "Rust is not installed (it is needed for the Metal renderer on iPad and iPhone): Set up tools installs it.", action: "install", size_mb: 300),
                ToolCheck(id: "xcode-first-launch", title: "Xcode's first launch", ok: false, problem: "Xcode needs its first-launch setup: click Open Xcode, then Install when it asks (it may ask for your Mac password), then come back here.", action: "open-xcode", size_mb: 0)]
            hub.settingUp = Job(state: .running, fraction: 0.5, text: "Installing CMake (2 of 3, about 89 MB)")
        }
    }
    let view: AnyView = state.hasPrefix("devices")
        ? AnyView(RootView(model: model, hub: hub, update: InstallerUpdate(), tab: 0, snapshot: true))
        : AnyView(ContentView(model: model).padding(24).frame(width: 640))
    let renderer = ImageRenderer(content: view.background(Color(nsColor: .windowBackgroundColor)))
    renderer.scale = 2
    guard let image = renderer.nsImage, let tiff = image.tiffRepresentation,
          let png = NSBitmapImageRep(data: tiff)?.representation(using: .png, properties: [:]) else {
        print("snapshot failed"); exit(1)
    }
    try? png.write(to: URL(fileURLWithPath: path))
}

/// install:SECONDS presses Install on every ready game of the (fake) device, waits, then quits
/// as Stop and Quit does; reopen starts like a fresh launch and reports what the window shows.
/// P2_AUTOTEST_REPORT: where the JSON report goes.
@MainActor
func autotest(_ mode: String) async {
    let hub = DeviceHub(), model = Installer()
    Session.hub = hub; Session.model = model
    func report(_ extra: [String: Any] = [:]) {
        var jobs: [String: [String: String]] = [:]
        for (id, job) in hub.jobs {
            let (kind, text): (String, String) = {
                switch job.state {
                case .waiting(let m): return ("waiting", m)
                case .running: return ("running", job.text)
                case .done(let m): return ("done", m)
                case .failed(let m): return ("failed", m)
                case .stopped(let m): return ("stopped", m)
                }
            }()
            jobs[id] = ["state": kind, "text": text]
        }
        var out: [String: Any] = ["jobs": jobs, "activity": hub.activity ?? "", "leftover": hub.leftover ?? "",
                                  "notice": hub.notice ?? "", "retry": hub.queue?.retryRow ?? "", "log": hub.log]
        out.merge(extra) { a, _ in a }
        if let path = ProcessInfo.processInfo.environment["P2_AUTOTEST_REPORT"],
           let data = try? JSONSerialization.data(withJSONObject: out, options: [.prettyPrinted, .sortedKeys]) {
            try? data.write(to: URL(fileURLWithPath: path))
        }
    }
    hub.restoreQueue()
    await hub.cleanLeftovers()
    await hub.refreshDevices(withStatus: false)
    if mode.hasPrefix("install") {
        let here = URL(fileURLWithPath: NSTemporaryDirectory())
        hub.library = Dictionary(uniqueKeysWithValues: allGames.map { ($0.id, LibraryEntry(folder: here, linux: here)) })
        hub.toolsChecked = true
        hub.picks = Set(allGames.filter { $0.needs == nil }.prefix(3).map(\.id))
        hub.install()
        let seconds = Double(mode.split(separator: ":").last ?? "5") ?? 5
        try? await Task.sleep(nanoseconds: UInt64(seconds * 1e9))
        let busy = Session.busy ?? ""
        Session.stopEverything()   // what Stop and Quit does
        report(["busy_at_quit": busy])
        exit(0)
    }
    if mode == "update" {   // Update Sauce on the fake device, to P2_SAUCE_LATEST from P2_SAUCE_ARCHIVE
        hub.checkLatestSauce()
        let here = URL(fileURLWithPath: NSTemporaryDirectory())
        hub.library = Dictionary(uniqueKeysWithValues: allGames.map { ($0.id, LibraryEntry(folder: here, linux: here)) })
        hub.hasXcode = true; hub.hasSigningAccount = true; hub.toolsChecked = true
        if let d = hub.device { await hub.readStatus(d) }
        let could = hub.canUpdateSauce
        hub.updateSauce()
        while hub.updateJob?.state == .running || hub.running { try? await Task.sleep(nanoseconds: 200_000_000) }
        var state = "", text = ""
        switch hub.updateJob?.state {
        case .done(let m)?: (state, text) = ("done", m)
        case .failed(let m)?: (state, text) = ("failed", m)
        default: state = "none"
        }
        report(["could_update": could, "update_state": state, "update_text": text])
        exit(0)
    }
    try? await Task.sleep(nanoseconds: 1_000_000_000)
    report()
    exit(0)
}

@main
enum Main {
    static func main() {
        let args = CommandLine.arguments
        if let i = args.firstIndex(of: "--snapshot"), i + 1 < args.count {
            MainActor.assumeIsolated { snapshot(to: args[i + 1], state: i + 2 < args.count ? args[i + 2] : "ready") }
            return
        }
        if args.contains("--self-test") {
            var p = 0.0
            for line in ["==> Checking tools", "==> Recompiling Portal 2", "  lifted 40/80 jobs: 1 functions", "  compiled 33/330 files, 12 s",
                         "==> Recompiling again", "==> Creating ~/Applications/Portal 2 Natif.app"] {
                p = Installer.progress(after: line, from: p)
                print(String(format: "%.3f", p), line)
            }
            p = 0.0
            for line in ["==> Recompiling Portal 2", "  compiled 330/330 files, 600 s", "==> Building the Puzzle Maker map compilers",
                         "  lifted 4/4 jobs: 1 functions", "  compiled 50/100 files, 60 s", "==> Creating ~/Applications/Portal 2 Natif.app"] {
                p = Installer.progress(after: line, from: p, mapCompilers: true)
                print(String(format: "%.3f", p), line, "(map compilers)")
            }
            return
        }
        // tests/test_installer_quit.py: the hub without a window, quit or force-quit mid-install, reopened.
        if let i = args.firstIndex(of: "--autotest"), i + 1 < args.count {
            let mode = args[i + 1]
            let app = NSApplication.shared
            app.setActivationPolicy(.prohibited)
            Task { @MainActor in await autotest(mode) }
            app.run()
            return
        }
        if args.contains("--unpack") {   // tests: unpack into sourceDir and exit
            do { try unpackSource(); print("unpacked into", sourceDir.path) } catch { print("error:", error.localizedDescription); exit(1) }
            return
        }
        if args.contains("--games") {   // tests: the table as this app decodes it
            for g in allGames {
                print([g.id, g.title, "\(g.appID)", "\(g.dataDepot)", "\(g.linuxDepot)", g.folder, g.markerFile,
                       g.linuxFiles.joined(separator: ","), "\(g.minutes)", "\(g.deviceBytes)", g.needs ?? "-",
                       g.extras.map(\.folder).joined(separator: ",")].joined(separator: "|"))
            }
            return
        }
        if args.contains("--detect") {
            print("command line tools:", hasCommandLineTools())
            let libs = steamLibraries()   // both tabs' scan
            print("steam libraries:", libs.map(\.path).joined(separator: ", "))
            for game in allGames {
                let g = game.findGame(libraries: libs), l = game.findLinux()
                let issue = g.flatMap { game.dataIssue($0) }
                let extras = game.extrasIn(g)
                let ready = g != nil && l != nil && issue == nil
                print("\(game.id): game \(g?.path ?? "-"), linux \(l?.path ?? "-")\(extras.isEmpty ? "" : ", with " + extras.joined(separator: ", "))"
                      + (ready ? " (ready)" : g == nil ? " (install it in Steam)" : l == nil ? " (needs: \(game.depotCommand))" : ""))
                if let issue { print(issue) }
            }
            print("apple silicon:", isAppleSilicon, "· Sauce profile expires:", sauceProfileExpiry().map { "\($0)" } ?? "-")
            return
        }
        InstallerApp.main()
    }
}

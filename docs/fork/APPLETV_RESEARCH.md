# Sauce on Apple TV: research

Research only, written 2026-11-25. Nothing has been built or installed. Every
tvOS limit below has a source link. Where no source gives a number, the
document says "not documented" and gives the closest evidence. The target is
Caspian's Apple TV 4K (2022), model A2737.

## 0. Which box this is

| Model | Connectivity | Storage | Source |
|---|---|---|---|
| A2737 | Wi-Fi only, **no Ethernet port** | 64 GB | [Apple: Identify your Apple TV model](https://support.apple.com/101605) |
| A2843 | Wi-Fi + Gigabit Ethernet (and Thread) | 128 GB | same page; [Apple TV 4K (3rd gen) tech specs](https://support.apple.com/en-us/111839) |

- Both models have the A15 Bionic and Wi-Fi 6 (802.11ax) with 2x2 MIMO ([tech specs](https://support.apple.com/en-us/111839)).
- Caspian's A2737 has **no Ethernet port** ([EveryMac A2737](https://everymac.com/systems/apple/apple-tv/specs/apple-tv-4k-3rd-gen-a15-2022-a2737-wifi-only-specs.html)), so the gigabit-Ethernet case below would need the A2843. The Apple TV has no other wired data port.
- Apple does not publish the RAM size. EveryMac says "third-party software analysis has determined that both have 4 GB of RAM", and gives the model ID as AppleTV14,1 ([EveryMac](https://everymac.com/systems/apple/apple-tv/specs/apple-tv-4k-3rd-gen-a15-2022-a2737-wifi-only-specs.html)).
- Wi-Fi link rates from Apple's deployment guide ([Wi-Fi specifications for Apple devices](https://support.apple.com/guide/deployment/dep268652e6c/web)):
  - 802.11ax on 5 GHz: maximum PHY rate 1200 Mbps, 80 MHz channel, 2 streams.
  - 802.11ax on 2.4 GHz: 195 Mbps.
  - Ethernet on the A2843: 1 Gbps.

## 1. Memory

**The limit is about 2 GB per app, and the entitlement Sauce uses on iPad does not exist on tvOS.**

- **Per-app limit.** Apple does not document this number. The best evidence is 9to5Mac, 15 Nov 2026 ([article](https://9to5mac.com/2026/11/15/new-apple-tv-ram-limit/)):
  - "Apps on Apple TV are limited by the operating system to 2 GB RAM each."
  - 9to5Mac "independently confirmed that an Apple TV app is force-quit by the system when it allocates more than two gigabytes".
  - `os_proc_available_memory` "reports a 2 GB limit to the app, the same value as the third-gen Apple TV". The third-gen box is Caspian's model.
  - The article says it is "unclear whether this is an intentional policy decision, or an oversight".
  - Follow-up coverage ([iMore](https://www.imore.com/music-movies-tv/apple-tv-4k/apple-tv-4ks-reported-ram-limit-is-frustrating-news-for-its-gaming-ambitions), [Mac Observer](https://www.macobserver.com/news/new-apple-tv-4k-has-8gb-ram-but-apps-and-games-are-still-limited-to-2gb/)) repeats 9to5Mac's numbers and adds no new measurement.
  - The exact cutoff in MB is not documented.
- **`com.apple.developer.kernel.increased-memory-limit`: not available on tvOS.**
  - Apple's page lists it for iOS 15+, iPadOS 15+ and visionOS 2.5+ only, with no tvOS ([Apple docs](https://developer.apple.com/tutorials/data/documentation/bundleresources/entitlements/com.apple.developer.kernel.increased-memory-limit.md)).
  - Even where it exists, Apple says it is "only available on some device models" and tells apps to check `os_proc_available_memory`.
- **`com.apple.developer.kernel.extended-virtual-addressing`: available on tvOS 14+** ([Apple docs](https://developer.apple.com/tutorials/data/documentation/bundleresources/entitlements/com.apple.developer.kernel.extended-virtual-addressing.md)).
  - It enlarges the address space only, not the memory limit. It helps if the runtime reserves large virtual ranges (guest heap, mmapped VPKs). It does not raise the 2 GB footprint ceiling.
- **Measured game footprints against a 2 GB ceiling.** These were measured on iPad, where the increased-memory entitlement was in effect. On Apple TV, GPU memory counts toward the same footprint, because Metal shares physical memory with the CPU.

| Game | iPad footprint (peak) | Against about 2 GB |
|---|---|---|
| Portal | about 1.4 GB | fits, about 0.6 GB headroom |
| Half-Life 2 | about 1.4–1.5 GB | fits, about 0.5 GB headroom |
| Left 4 Dead 2 c1m1 | 1.7–2.0 GB (2.4 GB at 4K) | borderline. Needs 1080p output, not 4K, and trimming |
| Portal Stories: Mel | about 2.0 GB (peak 2.8) | over at peak |
| Portal 2 | about 2.4 GB (peak 3.2) | over. Needs a memory diet of about 1 GB at peak |

- **What to do first:** run the existing `ios/probe` idea as a tvOS target and log `os_proc_available_memory()` on the A2737. That turns the press-reported 2 GB into a measured number for this exact box before any porting work starts.

## 2. Storage

**Only the app bundle and 500 KB of settings are kept for sure. Everything else the app writes can be deleted by the system.**

- **App bundle size.** "The maximum size for a tvOS app bundle [is] 4 GB" ([App Programming Guide for tvOS](https://developer.apple.com/library/archive/documentation/General/Conceptual/AppleTV_PG/)).
  - App Store Connect also lists 4 GB for the thinned tvOS app bundle, on tvOS 18 and earlier versions alike ([On-demand resources size limits](https://developer.apple.com/help/app-store-connect/reference/on-demand-resources-size-limits/)).
  - The limit for **development installs** from Xcode or `devicectl` is not documented. The 4 GB figure applies to App Store and TestFlight builds. Whether a larger dev-signed bundle installs is untested.
- **Persistent local storage.** "Your app can only access 500 KB of persistent storage that is local to the device (using the NSUserDefaults class). Outside of this limited local storage, all other data must be purgeable by the operating system when space is low" ([App Programming Guide for tvOS](https://developer.apple.com/library/archive/documentation/General/Conceptual/AppleTV_PG/)).
- **Caches directory.** "Downloaded data is not deleted while the app is running. However, when space is low and your app is not running, this data may be deleted. Do not use the entire cache space" ([same guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/AppleTV_PG/)).
  - When purging starts, and at what free-space threshold, is not documented beyond "when space is low and your app is not running".
- **Documents and Application Support.** On real hardware these are not writable:
  - A measurement on an Apple TV 4K running tvOS 26.5 found Documents "present, but writes are rejected". Application Support was missing and could not be created. Only `tmp` and `Library/Caches` accepted writes, while the simulator accepted all of them ([path_provider_tvos README](https://pub.dev/documentation/path_provider_tvos/0.0.3/)).
  - Older developer reports agree ([Couchbase forum](https://www.couchbase.com/forums/t/couchbase-lite-on-the-new-apple-tv-tvos/5215), [Unreal UE-78172](https://issues.unrealengine.com/issue/UE-78172)).
  - Consequence: the iOS layout (`Documents/<game>-userdata`, `Documents/game.log`, `Documents/launch.args`) must move to `Library/Caches`.
- **Real-world purging.** RetroArch users have reported downloaded games deleted from Caches. Developers of Delta and of RetroArch's tvOS work call this the main obstacle ([9to5Mac, May 2024](https://9to5mac.com/2024/05/20/tvos-restriction-game-apple-tv/)).
- **On-demand resources (ODR)** ([App Store Connect](https://developer.apple.com/help/app-store-connect/reference/on-demand-resources-size-limits/)):
  - Size limits: 512 MB per asset pack, 20 GB hosted. In-use resources were capped at 2 GB before tvOS 18; from tvOS 18 there is no limit.
  - ODR is **deprecated on tvOS 27 and later**, in favour of Background Assets.
  - ODR is hosted by the App Store, so it is only usable for App Store or TestFlight builds.
- **Background Assets (managed, Apple-hosted).**
  - Available on tvOS 26+ ([App Store Connect overview](https://developer.apple.com/help/app-store-connect/manage-asset-packs/overview-of-apple-hosted-asset-packs)), with up to 200 GB hosted.
  - "The system won't automatically remove your asset packs while your app is installed" ([Downloading Apple-hosted asset packs](https://developer.apple.com/tutorials/data/documentation/backgroundassets/downloading-apple-hosted-asset-packs.md)). That is the only documented persistent bulk storage on tvOS.
  - It needs TestFlight or App Store distribution, and it would mean uploading Valve's game files to Apple. That is redistribution of copyrighted data, so it is **not an option for Sauce**.
  - Whether self-hosted (non-Apple) managed packs keep the same no-removal guarantee on tvOS is not documented in what I read.
- **Can 7–15 GB sit permanently on the box?** Not with a documented guarantee.
  - The 64 GB A2737 has room for it, but the files can only live in `Library/Caches`, which the system may purge while Sauce is not running.
  - Workable approach: treat Caches as a rebuildable cache that refills from Caspian's Mac (see section 3). That fits tvOS's design rather than fighting it.
  - Portal at 4.4 GB is small enough that a one-time refill after a purge is tolerable.
  - An untested alternative for dev builds: put Portal's data inside the app bundle. The bundle is not purged, but 4.4 GB is over the 4 GB App Store figure, and the dev-install limit is not documented.

## 3. Caspian's streaming idea: on-demand game files over the LAN

**Fit with the existing code.** Every guest file open already passes through `runtime/paths.c`. `rt_read_path()` resolves a guest path to a host path. The posix layer (`runtime/posix.c`) handles open, stat and access through it. The "slim" pack (`P2_BASE_GAME_ROOT`, `pak01_slim.vpk` standing in for `pak01_dir.vpk`, with chunks taken from another game) shows that the layer can already swap the backing file for a VPK. Streaming adds one more backing source: "not on disk yet, fetch it."

**Sketch:**

1. **Always local.**
   - Every `*_dir.vpk`. These are small, and the engine reads the whole index at mount.
   - `gameinfo.txt`, configs, scripts, shader caches, and the files every map opens.
   - Build this "common set" on the Mac by logging opens during a few map loads. The project's own logs can do this.
2. **Server on the Mac.**
   - A small HTTP/1.1 server that supports byte ranges and serves the game folders.
   - Advertised over Bonjour (`_sauce._tcp`) with the Network framework (`NWListener`). The Apple TV finds it with `NWBrowser`, and an `NWConnection` handles the transfer.
   - The Mac is the right server. An iPhone or iPad app is suspended in the background, so it cannot serve reliably.
3. **Chunk files, `pakNN_NNN.vpk`.**
   - Create them locally as sparse files of the right size, with a bitmap of 1 MB blocks that are present.
   - The posix `read`/`pread` path checks the bitmap and fetches any missing ranges before returning.
   - The engine only ever sees normal files. `stat` sizes are correct, so VPK offsets in `_dir.vpk` stay valid.
4. **Loose files** (`maps/*.bsp`, loose materials and sounds): fetch the whole file when it is first opened, then rename it into Caches.
5. **Per-map prefetch.**
   - On the Mac, record which VPK ranges and loose files each map touches, and save them as a manifest such as `maps/<name>.ranges`.
   - When `map`/`changelevel` starts, the client fetches the whole manifest before the engine's load begins. Source then loads from local disk.
   - This matters because Source also opens files lazily mid-game: sounds, models, and the next map in a level transition. A blocking network read there would cause stutter. With a manifest, a mid-game miss should be rare.
6. **Eviction.**
   - Keep an LRU of blocks under a budget (for example 15–20 GB of Caches). Never evict the current map or the common set.
   - If tvOS purges Caches while Sauce is closed, the next launch simply refetches the common set.
7. **Offline fallback:** if no server is found, play whatever maps are fully cached.

**Local-network permissions on tvOS:**

- Apple's TN3179 table lists **tvOS as not supporting local network privacy** ([TN3179](https://developer.apple.com/tutorials/data/documentation/technotes/tn3179-understanding-local-network-privacy.md)). There is no local-network prompt on the Apple TV.
- `NSLocalNetworkUsageDescription` is needed on iOS, but on tvOS it does nothing. It is harmless to include for a shared plist.
- Listing `_sauce._tcp` under `NSBonjourServices` is still sensible.
- The multicast entitlement is described as an iOS restriction. Plain Bonjour browsing for one named service type does not need it on iOS. Its status on tvOS is not documented.
- Network framework is available on tvOS. The Mac-side server will trigger the macOS local-network prompt once ([TN3179](https://developer.apple.com/tutorials/data/documentation/technotes/tn3179-understanding-local-network-privacy.md), macOS 15+).

**Load-time estimate.** The bytes fetched for one map are assumed to be 300–800 MB. These are assumed sustained application throughputs, not measurements:

| Link | Assumed throughput | 300 MB | 800 MB |
|---|---|---|---|
| Wi-Fi 6, Apple TV close to router, Mac wired | about 850 Mbps (best case) | about 3 s | about 7.5 s |
| Wi-Fi 6, typical room, Mac wired | about 400 Mbps | about 6 s | about 16 s |
| Wi-Fi 6, Mac also on Wi-Fi (same airtime shared) | about 200 Mbps | about 12 s | about 32 s |
| Poor Wi-Fi (other room, interference) | about 100 Mbps | about 24 s | about 64 s |
| Gigabit Ethernet (A2843 only, not Caspian's box) | about 940 Mbps | about 2.6 s | about 6.8 s |

- **Where the throughput figures come from:**
  - The 1200 Mbps ceiling is Apple's PHY rate ([deployment guide](https://support.apple.com/guide/deployment/dep268652e6c/web)).
  - Independent tests of 2x2, 80 MHz Wi-Fi 6 clients report about 800–940 Mbps TCP at close range ([IFIP Networking 2024](https://dl.ifip.org/db/conf/networking/networking2024/1571003562.pdf) measured about 931 Mbps down; [Evan McCann](https://evanmccann.net/blog/2021/6/understanding-6-ghz-wifi-speed) gives 800–900 Mbps ideal).
  - The 400, 200 and 100 Mbps rows are my assumptions for normal homes. There is no Apple TV–specific Wi-Fi benchmark. Apple Community threads show some Apple TVs far below other devices on the same network ([example](https://discussions.apple.com/thread/253760277)).
  - The gigabit row is line rate minus TCP/IP overhead, also an assumption.
- **Comparing with today:** these times come on top of the normal Source load time, which is CPU-bound on device. They apply only to the first visit to a map. Later loads come from the local cache.
- **Bigger saving:** for linear games (HL2, Portal), prefetch the next map's manifest in the background while the player is still on the current map. The changelevel then costs nothing extra.

## 4. The port: what carries over and what changes

**Carries over:**

- **Recompiled C.** The guest code is already arm64 C, and the Apple TV runs arm64. In principle it is a rebuild for the `appletvos` SDK, which is installed on the Mac (tvOS 27.2 SDK in `xcodebuild -showsdks`), using `-target arm64-apple-tvos…`. Any iOS-only APIs in `runtime/ios/*` will show up as compile errors.
- **Metal and libd3d9metal.** Metal is present on tvOS. Whether mtld3d uses any API that is unavailable on tvOS is unknown until it compiles.
- **GameController.** The framework is on tvOS 9+ (for example [`GCControllerDidConnectNotification`](https://developer.apple.com/tutorials/data/documentation/gamecontroller/gccontrollerdidconnectnotification.md)). The existing controller code should mostly carry over.
- **SDL2.** SDL2 has tvOS support. Its hints include `SDL_HINT_TV_REMOTE_AS_JOYSTICK`: the Siri Remote appears as a 2-axis, 2-button joystick by default, or as arrow/enter/escape keys ([SDL wiki](https://wiki.libsdl.org/SDL2/SDL_HINT_TV_REMOTE_AS_JOYSTICK)). `ios/deps/build.sh` would need a tvOS slice of SDL2 and FreeType. gl4es and the OpenGL ES path can probably be dropped and the d3d9 → Metal path kept.

**Changes:**

- **No touch.** The touch overlay, back-tap and on-screen controls are removed. The game is controller-first. The Siri Remote can drive menus only, through SDL's remote-as-joystick mode or arrow keys. It is not realistic for playing these games.
- **Remote and controller input versus the focus engine.** On tvOS the controller also drives the system UI. To receive raw controller input without the focus engine taking it, the root view controller should be a [`GCEventViewController`](https://developer.apple.com/tutorials/data/documentation/gamecontroller/gceventviewcontroller.md) with `controllerUserInteractionEnabled = NO`. Check whether SDL's tvOS view controller already does this.
- **Menu / Back button.** The Back button on the Siri Remote or controller exits to the home screen unless the app handles it. It must map to the game's Escape/pause.
- **Info.plist.** Set [`GCSupportsControllerUserInteraction`](https://developer.apple.com/tutorials/data/documentation/bundleresources/information-property-list/gcsupportscontrolleruserinteraction.md) (tvOS 9+) and declare ExtendedGamepad.
- **Lifecycle.** When dismissed, a scene goes to the background and then suspended. "UIKit can disconnect a background or suspended scene at any time to reclaim its resources" ([UIKit: Managing your app's life cycle](https://developer.apple.com/documentation/uikit/managing-your-app-s-life-cycle)). Sauce should autosave or pause on resign-active, and expect to be cold-launched again. The iPad build's existing pause-on-background path is the model.
- **Storage paths.** Saves, logs, `launch.args` and settings move from Documents to `Library/Caches` (section 2). Saves are also purgeable, so iCloud is the durable copy. The iOS app already syncs saves to iCloud Documents. Whether iCloud Documents (as opposed to CloudKit or key-value storage) is available on tvOS needs checking. Older guidance says tvOS supports only the database layer ([9to5Mac comments summary](https://9to5mac.com/2024/05/20/tvos-restriction-game-apple-tv/)).
- **Getting data and settings onto the box.** There is no Files app and no file sharing on tvOS. Options:
  - `xcrun devicectl device copy to --domain-type appDataContainer` (the local `devicectl` 651.32 help lists this domain; tvOS support untested).
  - The streaming client in section 3, which is the cleaner route.
- **Settings.** I did not find a source on whether tvOS reads a Settings.bundle, so treat it as unverified. An in-game options menu or `launch.args` is safer.
- **Output.** Render at 1080p by default, 4K optional. L4D2 alone grew by about 0.4 GB at 4K on iPad.

## 5. Signing and installing from the Mac (paid team)

- **Pairing.**
  - On the Apple TV, open Settings > Remotes and Devices > Remote App and Devices. In Xcode's Devices and Simulators window the Apple TV appears under "Discovered". Click Pair and enter the PIN shown on the TV ([Xcode Help: Pair a wireless device](https://help.apple.com/xcode/mac/current/en.lproj/devbc48d1bad.html)).
  - The Mac and the Apple TV must be on the same network.
  - Forum reports of pairing failures are usually fixed by removing the Mac from the TV's list and pairing again ([forum thread](https://developer.apple.com/forums/thread/681984)).
  - Developer Mode may need enabling on the TV after the first pairing. I did not find a tvOS-specific source for this.
- **devicectl.**
  - The local `xcrun devicectl` (651.32) has `manage pair`, `device install app`, `device process launch` and `device copy to`, all addressed to "a device discovered by CoreDevice". No Apple TV is paired with this Mac yet.
  - Apple's documentation does not say explicitly whether tvOS devices are fully supported. Verify by running `xcrun devicectl list devices` after pairing in Xcode. If the TV appears there, `tools/ios_device.sh`-style scripts should work with a tvOS bundle.
- **Provisioning.**
  - Automatic signing registers the Apple TV's UDID with the team and creates a tvOS development profile. tvOS profiles are separate from iOS ones; Xcode handles this.
  - The signing project (`ios/signing/project.yml`) needs a tvOS target. Use the same bundle ID and remove `increased-memory-limit` from its entitlements: tvOS does not list that entitlement, so leaving it in risks a provisioning error (section 1).
  - Optionally add `extended-virtual-addressing`.
- **Same bundle ID / App ID.** Yes. Apple's universal-purchase guidance says iOS and tvOS apps in one purchase "must use the same App Store Connect record and have the same bundle ID" ([Xcode Help: universal purchase](https://help.apple.com/xcode/mac/current/en.lproj/deve7aa31da5.html)). One explicit App ID can therefore cover both platforms. The iCloud container and capabilities attach to the same App ID.

## 6. Verdict and size

**Realistic for Portal and Half-Life 2. Borderline for Left 4 Dead 2. Not realistic for Portal 2 or Mel without a large memory diet.**

The hard limit is memory, not storage or bandwidth:

- tvOS appears to kill apps at about 2 GB ([9to5Mac](https://9to5mac.com/2026/11/15/new-apple-tv-ram-limit/)).
- There is no increased-memory entitlement on tvOS ([Apple](https://developer.apple.com/tutorials/data/documentation/bundleresources/entitlements/com.apple.developer.kernel.increased-memory-limit.md)).
- Portal 2 peaks at 3.2 GB on iPad.

Storage can be handled. It is purgeable, but a cache refilled from the Mac makes that tolerable.

**Which game first: Portal.**

- Memory: about 1.4 GB, the most headroom.
- Data: 4.4 GB, the smallest set. It can sit whole in `Library/Caches` without streaming, so the first round tests only the port.
- Second: Half-Life 2 (about 1.5 GB, 11.1 GB of data). This is where the streaming layer starts to pay off.
- Third: L4D2, at 1080p and only after measuring.
- Last: Portal 2 and Mel, which would need lower texture budgets, smaller runtime heap reservations, and similar savings.

**Rough rounds:**

1. **Probe round.** Pair the TV and get a tvOS target signed with the same bundle ID. On the A2737, log:
   - `os_proc_available_memory`
   - which directories are writable
   - the dev-install bundle size limit
   - whether devicectl works.
2. **Build round.** tvOS slices of SDL2 and FreeType, the runtime and mtld3d built for `appletvos`, paths moved to Caches, Portal booting to the menu.
3. **Input and lifecycle round.** Controller-only, Menu/Back mapped to pause, remote for menus, suspend and resume, 1080p default. Ends with Portal playable from a full local copy.
4. **Streaming round(s), 1–2 rounds.** Mac server, Bonjour, sparse VPK chunks in `runtime/paths.c`/`posix.c`, per-map manifests, LRU eviction. Ends with HL2 playable.
5. **Memory round(s).** L4D2, then attempts at Portal 2 and Mel.

Estimate: **3 rounds to Portal on the TV, about 5 rounds to HL2 streaming, and 6 or more for anything beyond that.** If the round-1 probe measures well under 2 GB, or Apple raises the cap, re-plan from there.

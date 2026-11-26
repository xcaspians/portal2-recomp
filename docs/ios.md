# iOS port

Portal 2 runs on an iPhone 15 Pro Max (iOS 27): menus, gameplay, level
transitions, voice lines, on-screen touch controls. Half-Life 2 has had a
first short test on the same phone, at 60 fps; see [Half-Life 2](#half-life-2).

Tested devices:

| Device | Chip | Who | What |
|---|---|---|---|
| iPhone 15 Pro Max | A17 Pro | upstream | Portal 2 played at 60 fps; Half-Life 2 short session |
| iPhone 17 Pro Max | A19 Pro | upstream | Portal starts and renders |
| iPhone 18 Pro (iPhone19,2) | A20 Pro | this fork (Caspian) | the fork's device builds |
| iPad mini (A20 Pro, iPad19,1) | A20 Pro | this fork (Caspian) | the fork's device builds |

Older phones (A15, A16) have not been run yet; see [Older phones](#older-phones).

## Metal renderer (default)

Instead of gl4es, the app can render through the native Direct3D 9 path
described in [d3d9-bridge.md](d3d9-bridge.md): `shaderapivk.so` → bridge →
libd3d9metal (mtld3d) → Metal. It is the default when the app is built with the
library and the GPU supports BC textures; otherwise the app uses gl4es. It
is about 20–25 % faster than OpenGL on an iPhone 15 Pro Max
([measurements](../STATUS.md#measurements)) and fixes the too-dark image of
the gl4es path.

`./setup.sh --ios` builds and links the library when Rust from rustup is
installed (Homebrew's `rust` has no iOS target; the app then uses OpenGL). By hand:

```sh
# in the mtld3d checkout (branch feat/native-host), once per mtld3d change:
make native-ios PROFILE=production        # native/target/aarch64-apple-ios/production/libd3d9metal.a
# here: gameprep must have kept shaderapivk.so
python3 tools/build_ios.py --target device --release \
    --d3d9metal ../mtld3d/native/target/aarch64-apple-ios/production/libd3d9metal.a
tools/ios_device.sh install
tools/ios_device.sh prep                  # the app's module layout includes shaderapivk.so now
```

`"d3d9metal_ios": {"device": "/path/to/libd3d9metal.a"}` in
`config/local.json` replaces `--d3d9metal`. The library is linked
statically (iOS does not `dlopen` a dylib outside the signed app):
`runtime/d3d9.c` is compiled with `RT_D3D9METAL_STATIC`. Without it the app is
OpenGL only and the setting below falls back to OpenGL.

On the phone: Settings > Portal 2 (or Half-Life 2) > Renderer chooses Metal (default) or OpenGL;
`P2_RENDERER=opengl` or `=d3d9` in `tools/ios_device.sh run` overrides it. `Documents/game.log` (with the
Log setting) then shows `[ios] renderer: Metal (...)`,
`d3d9: libd3d9metal (linked in) ready` and `[ios] Metal view: ...`.

- Device check first (`runtime/ios/metal_ios.m`): Source's compressed
  textures are DXT, and mtld3d creates BC textures with no CPU fallback
  (gl4es decompressed them). A GPU whose `supportsBCTextureCompression` is
  false stays on OpenGL, with a log line.
- Resolution: the Metal view takes the app's render scale (Resolution
  setting), like the GL view, so the drawable is the game's back buffer
  size and mtld3d does not resample.
- Frame rate: `present.maxFps` follows `P2_IOS_MAX_FPS` (60 by default) and
  auto power saving, see "Frame rate cap"; the 120 Hz display request is the
  same as on OpenGL. The FPS counter and
  back taps count frames on `Present` (`rt_d3d9_on_present`).
- Upscaling (MetalFX): Settings > Upscaling puts `render.scale=0.75` or
  `0.5` in `MTLD3D_CONFIG` (`P2_IOS_UPSCALE`; a `render.scale` you set in
  `MTLD3D_CONFIG` yourself comes later and wins). mtld3d then rasterizes the
  scene at that fraction of the game's back buffer and enlarges it with the
  MetalFX spatial scaler, with a texture LOD bias so textures keep their
  detail (`render.lodBias`, on by default). It multiplies on top of
  Resolution: the back buffer is the screen in points × the Resolution scale,
  so ×2 with Quality renders 1398×645 on an iPhone 15 Pro Max, upscaled to
  1864×860, which the display then stretches to 2796×1290; Native with
  Quality renders 2097×968 (about the pixel count of ×2.25) and MetalFX
  produces the full native image. The interface is scaled with the world.
  Ignored with OpenGL (`[ios] MetalFX : agrandissement … ignoré`); the log
  says `[ios] MetalFX : rendu à 75 % du tampon arrière` when it is active.
  Not measured yet on the phone.
- Shader/pipeline cache: `Library/Caches/d3d9metal` (`P2_D3D9_CACHE` to
  move it). Memory caps: the same `MTLD3D_CONFIG` defaults as on macOS.
- Simulator: not available. mtld3d links MetalFX, which the iOS Simulator
  SDK does not have.
- `make d3d9metal-check D3D9METAL_A=…/libd3d9metal.a` drives the library
  alone, linked statically as in the app (macOS build of the library); the
  same source links against the iOS library.

## OpenGL renderer (fallback)

togl (D3D9 → OpenGL 2.1) → `runtime/gl.c` → `runtime/ios/gl_ios.m` → gl4es
(static, `ios/deps/gl4es-ios.patch`) → OpenGL ES 3 (OpenGLES.framework).

- `runtime/ios/essl3.c` rewrites every GLSL ES 1.00 shader emitted by gl4es as
  ES 3.00: sRGB emulation (decode per texture unit, encode per output),
  texture borders, and BGRA color swizzling for the `vN` attributes.
- `runtime/ios/srgb.c` tracks sRGB/border state between togl and gl4es and sets
  the uniforms before each draw.
- `gl_ios.m`, at the gl4es → ES boundary: ES 3 sized formats, 16-bit textures
  as half floats, unsynchronized `glBufferSubData` writes
  (`glMapBufferRange` + `GL_MAP_UNSYNCHRONIZED_BIT`).

## Building and running

What `./setup.sh --ios` runs, step by step (`python3 tools/gameprep.py`
first if it was not done for macOS):

```sh
P2_IOS_DEPS=deps/ios-deps ios/deps/build.sh device      # SDL2, FreeType, gl4es (once, or after changing the patch)
python3 tools/build_ios.py --target device --release --deps deps/ios-deps/device   # for playing: -O2 without memory checks
tools/ios_device.sh install                       # installs on the connected iPhone (unlocked, Developer Mode on)
tools/ios_device.sh data                          # copies the game data (~11.5 GB, once)
tools/ios_device.sh run [seconds] [VAR=value…]    # launches with P2_FRAME_LOG=1, waits (180 s by default), pulls back Documents/game.log into build/ios/device/
```

`ios/deps/build.sh` writes to `P2_IOS_DEPS` (default `../work/ios-deps`) and
`build_ios.py --deps` reads the matching `<prefix>/device`; `setup.sh` uses
`deps/ios-deps` (`P2_DEPS` moves it). `P2_DEVICE=<UDID or name>` picks the
iPhone for `tools/ios_device.sh` when several are connected.

Simulator: `ios/deps/build.sh sim`, `python3 tools/build_ios.py`, then
`tools/run_ios_sim.sh <seconds>` (`P2_SIM_DEVICE=<UDID>` for a different
simulator; `P2_SIM_AUDIO=1` to hear the sound on the Mac's speakers, muted
otherwise). The simulator renders in software and interpolates triangles
clipped at the near plane incorrectly (`ios/probe/clip_test.c`): its
artifacts do not exist on a real device. The Metal renderer is not available
in the simulator.

Signing (free account): 7-day profiles obtained through
`xcodebuild -allowProvisioningUpdates` on `ios/signing/project.yml`, then
`ios/signing/sign.py` (called by `build_ios.py`). Launching fails if the
iPhone is locked.

The app identifier must belong to your Apple account: set `ios_bundle_id` in
`config/local.json` (or `P2_BUNDLE_ID`, which applies to both games, so
`setup.sh --ios` refuses it when it builds both), read by every tool through
`tools/ios_bundle_id.py`; the probe uses the same identifier plus `.probe`.
First profile, once per identifier (then again every 7 days):

```sh
cd ios/signing
export P2_BUNDLE_ID=$(python3 ../../tools/ios_bundle_id.py)   # --game hl2 for Half-Life 2
xcodegen
xcodebuild -project P2Signing.xcodeproj -scheme Portal2 -destination generic/platform=iOS \
  -allowProvisioningUpdates DEVELOPMENT_TEAM=YOURTEAMID build
```

Replace `YOURTEAMID` with your team ID (Xcode > Settings > Accounts, or the
`OU` field of your Apple Development certificate). `setup.sh` takes the team
of the first Apple Development certificate in the keychain: with several Apple
accounts in Xcode, set `P2_TEAM_ID` to the one you want.

Saves: `tools/ios_device.sh saves status|pull|push` copies them between the Mac
and the iPhone over the cable ([details](usage.md#saves-between-mac-and-iphone-portal-2));
Portal 2 only for now.

Custom maps, addon folders and console tweaks: [mods.md](mods.md).

`Documents/launch.args` in the app's container adds arguments
(one per line), for example `+map` followed by `sp_a2_laser_intro`.

The game language is written into the app's `launch.args` when it is built
(`-language <name>`, chosen as on the Mac: see [Game language](usage.md#game-language)).
To change it, set `"language"` in `config/local.json` and rebuild.

## Touch controls (Portal 2)

Hidden in menus and when a controller connects (`runtime/ios/touch_layout.c`).

| Where | Button |
|---|---|
| left half | move stick |
| right half | camera stick or trackpad |
| right edge | jump (middle), use (bottom), crouch (left of them) |
| top left and right | blue and orange portals, only when back taps are off |
| top right, small | pause; ✗ to report a wrong back tap while recording taps |
| bottom left, small | camera mode (stick or trackpad) |

Portals are fired by tapping the back of the phone (see [Back taps](#back-taps)).

## Settings (iOS Settings app)

The game's page in the Settings app ("Portal 2" or "Half-Life 2"; `ios/Settings.bundle`, copied into the app
by `tools/build_ios.py`). At launch, `runtime/ios/settings.c` turns each
changed setting into a `P2_*` variable; a variable that is already set (devicectl,
simctl) takes precedence, and a setting that was never touched leaves the code's
default in place. The settings that are also in the in-game menu (see
below) apply when you come back to the game after changing them there; the
others (Resolution, Renderer, Upscaling, On-screen controls, Trackpad camera
and acceleration, FPS counter, Auto power saving, Log, Record taps) apply at
the next launch.

**Source of truth: the app's preferences (`NSUserDefaults`).** The Settings
app and the in-game menu (pause menu, gear button; see "In-game settings
menu") read and write the same preferences, so each shows what the other set.
A change made in the game is written there, applied to its `P2_*` variable at
once and announced to the parts that read it (`runtime/ios/live_settings_ios.m`:
back taps, gyro, frame rate cap, touch overlay). Changes made in the Settings
app are applied the same way when the game comes back to the foreground. The
items of the in-game menu, their ranges and defaults are in
`runtime/ios/settings_menu.c`; `tests/checks_ios_settings.c` checks that they
match `ios/Settings.bundle`.

The page is in English, and in French on a device set to French
(`ios/Settings.bundle/fr.lproj/Root.strings`).

| Setting | Variable | Values |
|---|---|---|
| Resolution | `P2_IOS_RENDER_SCALE` (+ the game's `-w`/`-h`) | native, ×2 (default), ×1.5, ×1 |
| Frames per second | `P2_IOS_MAX_FPS` | 30, 40, 60 (default), 120, auto (the screen's maximum: 120 on ProMotion, 60 otherwise); see "Frame rate cap" |
| Auto power saving | `P2_IOS_POWER_SAVE` | on by default: at most 30 fps in Low Power Mode or when the phone is hot |
| Renderer | `P2_RENDERER` | Metal: `d3d9` (default), `opengl`; see "Metal renderer" |
| Upscaling (MetalFX) | `P2_IOS_UPSCALE` | Off (1, default), Quality (0.75), Performance (0.5); Metal only, see "Metal renderer" |
| FPS counter | `P2_SHOW_FPS` | off by default; also in the gear menu; a tap on the counter hides it |
| On-screen controls | `P2_TOUCH_CONTROLS` | automatic (hidden while a controller is connected; a touch shows them, the next controller input hides them again), always, never, off with a controller (`pad`: hidden and touches ignored while a controller is connected) |
| Trackpad camera | key `P2TouchTrackpad` | also toggled by the button at the bottom left (top left in Half-Life 2) |
| Camera speed | `P2_TOUCH_LOOK` | ×0.5 to ×3 |
| Trackpad acceleration | `P2_TOUCH_ACCEL` | none (1) to strong (4), normal 2.5 |
| Button opacity | `P2_TOUCH_OPACITY` | slider, 0 to 1 (100 %, default): buttons and sticks, on top of their own transparency |
| Button size | `P2_TOUCH_SIZE` | slider, 0.75 to 1.5 (default 1): every button and stick scaled around its anchor, moved back inside the screen if needed (`rt_touch_place_scaled`) |
| Gyro aiming | `P2_GYRO` | on by default in Half-Life 2, off in Portal 2; see "Gyro aiming" |
| Gyro speed | `P2_GYRO_SPEED` | ×0.5 to ×2, normal 1 |
| Back taps | `P2_BACKTAP` | enabled by default |
| Vibrate when firing | `P2_HAPTICS` | on by default: a light Taptic Engine tap for each portal fired from the overlay (button or back tap) |
| Tap sensitivity | `P2_BACKTAP_SENSITIVITY` | slider, 0.5 to 3, default 1: thresholds are divided by this value (higher catches lighter taps) |
| Swap left and right | `P2_BACKTAP_INVERT` | |
| Portal buttons | `P2_PORTAL_BUTTONS` | automatic (when back taps are off), always, never |
| Record taps | `P2_BACKTAP_RAW=Documents/backtap/` | off; see [backtap.md](backtap.md) |
| Log | `P2_LOG_FILE=Documents/game.log` | |

## In-game settings menu (Portal 2)

A **Touch Controls** gear button shows at the top right **while a game menu
is open: the main menu or the pause menu** (never in gameplay); Portal 2 and
Mel. Every item works from the main menu. It opens a
dark, translucent panel drawn by the runtime over the game
(`runtime/ios/settings_menu_ios.m`, items in `runtime/ios/settings_menu.c`):

| Section | Items |
|---|---|
| On-screen buttons | Button opacity, Button size, Show portal buttons |
| On-screen buttons | also **Edit button layout** (below) |
| Back taps | **Back taps** on/off (the Settings app's switch; off shows the portal buttons), Tap sensitivity (0.5 to 3), **Calibrate back taps** ([backtap.md](backtap.md#calibration)), Swap left and right, Vibrate when firing |
| Camera | Camera speed, Gyro aiming, Gyro speed |
| Display | Frames per second (including Auto) |
| Maps & Mods | **Community Maps** (Steam Workshop browser and downloader, [mods.md](mods.md#community-maps-steam-workshop-in-the-game)), then the custom maps found in `portal2/maps/custom` and `maps/workshop` (game and user folders) and `portal2_dlcN/maps` (N ≥ 3); a tap starts one ([mods.md](mods.md)) |
| | Reset to defaults (press twice), the fork's version at the bottom |

- **Live:** every change applies at once and is saved in the app's
  preferences (see "Source of truth" above), so it is also on the Settings
  app page and kept after a relaunch. The touch buttons are drawn behind the
  open panel so opacity and size changes show while you drag. One exception:
  with the Metal renderer the frame rate cap applies on the next launch
  (mtld3d reads it once); the panel says so under that item.
- **Edit button layout:** the panel closes and the game's buttons show where
  they are; drag one to move it, its white dot to resize it (0.6× to 1.8×);
  the sticks' resting areas show as guides. Reset, Cancel, Save. A button
  stays whole on screen, snaps to an 8-point grid and is pushed out of the
  sticks' resting areas (`rt_touch_custom_*` in `touch_layout.c`). Saved per
  device kind (phone, iPad) and per game; Button opacity and size still apply
  on top.
- **The game's own Community button:** Community Test Chambers > Play
  Single Player Chambers opens Community Maps instead of the game's empty
  dialog (no Steam on iOS). The runtime reads the game's UI (VGUI's panel
  tree, a visible `CommunityMapDialog`), closes that dialog like a player
  would (Escape) and opens Community Maps; Back returns to the game's
  Perpetual Testing screen. Nothing in the game is changed and no Steam
  interface is faked.
- **Touch:** sliders, switches and segmented choices; the ✕ closes it.
- **Controller:** the options button (View, Select or −; its name is shown
  under the gear when a controller is connected) opens and closes it; d-pad or
  left stick up and down move between items, left and right change the
  focused item; A flips a switch, steps a choice or presses a button; B
  closes. While the panel is open the game gets no controller presses or stick
  motion (`sdl_ios.m`, rule in `rt_menu_pad_dropped`), only releases and a
  stick's return to centre, so nothing stays held; a button still held when it
  closes is kept from the game until released. In the pause menu the options
  button is the panel's alone.
- **When the gear shows:** game UI visible (`IEngineVGui::IsGameUIVisible`)
  and a level loaded: `IVEngineClient::IsInGame` and not
  `IsLevelMainMenuBackground` (`VEngineClient015`, entries 25 and 90, read
  from the symbols of the Mac edition's `engine.dylib`; `game_state.c`
  checks them at run time and leaves the gear off if they don't behave).
  Portal 2 only: Half-Life 2 and Portal use other interface versions.
- **Maps & Mods:** the list is read each time the panel opens
  (`runtime/ios/mods_list.c` turns a file into the name the console loads,
  and refuses any name that could carry a second command). A tap sends
  `map <name>` through `IVEngineClient::ClientCmd_Unrestricted` (entry 108),
  which the engine runs at its next frame, and closes the panel.
- `P2_SETTINGS_MENU_OPEN=1` opens the panel at the first pause (simulator
  checks, where no finger can press the gear); `=map` also starts the first
  map of Maps & Mods; `=display` scrolls to "Defaults for <device>". Checked in the simulator on 24 November 2026: a map in
  `portal2_dlc3/maps` was listed, started and loaded, so the runtime mounts
  addon folders (still to confirm on a device).
  `=main` opens it on the main menu, `=editor` opens the layout editor
  there, `=display` and `=end` scroll to the defaults row or the bottom.
  `P2_TOUCH_TEST_CLICK=s,x,y;s,x,y` clicks several times (seconds after the
  first menu, points).

The touch overlay must be on (On-screen controls not set to "Never") for the
gear to exist; the Settings app page always works.

## iCloud saves (Portal 2)

Saves and their screenshots go between your iPhone and iPad through the app's
iCloud Drive container (`runtime/ios/icloud_saves_ios.m`, rules in
`runtime/ios/save_sync_rules.c`, checked by `tests/checks_save_sync.c`). On
by default: **iCloud saves** in the in-game menu (with "Last synced" under
it) and on the Settings app page (`P2_ICLOUD_SAVES`).

- **What:** the `*.sav` and `*.tga` files of
  `Documents/portal2-userdata/portal2/SAVE/<id>/`, to
  `<container>/Documents/SAVE/<id>/`. Never settings or video config.
- **When:** at launch, about five seconds after the game writes a save (the
  folder is checked every 10 s), when you leave the app, and when you come
  back to it (that's when another device's new saves arrive).
- **Rules:** a save changed on one side only is copied to the other. A save
  changed on both sides since the last sync (or two different saves of the
  same name, never synced) is a **conflict: both are kept**. The other
  device's copy is renamed with that device's name and the time
  (`quick_iPad-mini_2026-11-24_1405.sav`) on both devices, and the
  game's Load list shows the newer one first, since it sorts by date.
  Two devices uploading the same name at the same moment (an iCloud conflict)
  are kept the same way. **Deleting a save never deletes it elsewhere.** A
  local save about to be replaced by a download is first copied to
  `Documents/portal2-userdata/icloud-backups/<date>/` (two weeks kept). The
  state of both sides at the last sync is in
  `Documents/portal2-userdata/icloud-sync.json`.
- **Safety rules** (from the round 4 audit, `rt_sync_after` in
  `save_sync_rules.c`, checked over two syncs in a row):
  - a save iCloud hasn't finished downloading is left alone until it has
    arrived (never uploaded over, never recorded);
  - a copy that fails (iCloud or the device full, the app killed) leaves the
    record as it was, so the next sync sees the change again; the menu then
    says some saves couldn't be copied, and it retries every 15 s;
  - a save deleted on this device stays deleted on the next syncs too;
  - conflict copies never overwrite each other (`-2`, `-3`… when the name is
    taken), and iCloud's own conflict versions are only dropped once copied.
- **Mac:** left out. The Mac app isn't provisioned for iCloud and goes
  through real Steam, which may sync its own saves with Steam Cloud; the
  cable sync (`tools/ios_device.sh saves`) stays the Mac ↔ iPhone route.
- Log lines: `[ios] iCloud (<why>) : N envoyées, N reçues, N conflits gardés
  en double, N en attente`, and `conteneur indisponible` when iCloud can't
  be reached.

**One-time setup (paid developer account).** The app needs the iCloud
capability, which only a provisioning profile can grant:

1. Add `"ios_icloud": true` to `config/local.json` (top level).
2. Connect and unlock the iPhone, then run `./setup.sh --ios --game portal2`.
   Its signing step uses `ios/signing/project-icloud.yml`: Xcode turns on
   iCloud Documents for the app's identifier and creates the container
   `iCloud.<identifier>` on your account (**permanent**: Apple never deletes
   a container). `ios/signing/sign.py` then narrows the profile's iCloud
   entries to that container (`narrow_entitlements`, checked by
   `tests/test_ios_sign.py`).
3. If that step fails with a capability error (on 24 November 2026 it turned
   iCloud on for the identifier but attached no container: the profile had
   iCloud with an empty container list, and the build stopped with
   "doesn't match the entitlements file's values for ...
   icloud-container-identifiers"): open
   `ios/signing/P2Signing.xcodeproj` in Xcode, select the Portal2 target >
   Signing & Capabilities, check that iCloud shows "iCloud Documents" with
   the container `iCloud.<identifier>` ticked (the existing one: never press
   + to make a new container), build once (⌘B), then run step 2 again.
   Before round 25 the tab could neither show nor add iCloud: XcodeGen 2.46
   writes the project's nested `SystemCapabilities` as text, and
   `project-icloud.yml` now runs `ios/signing/capabilities.py` after
   generation to make it a real dictionary
   (`tests/test_ios_signing_project.py`).
4. Install on the iPad too: `P2_DEVICE=<iPad UDID> tools/ios_device.sh install`.

Until then the app runs as before, and the menu says "iCloud isn't
available"; nothing is deleted or moved.

### Frame rate cap

`P2_IOS_MAX_FPS` (Frames per second) is the ceiling, read at launch by
`runtime/ios/power_ios.m` (`auto` becomes 120 on a ProMotion screen and 60
otherwise, from `UIScreen.maximumFramesPerSecond`; `rt_ios_fps_choice` in
`settings.c`, checked by `tests/checks_ios_settings.c`). 60 stays the default
until 120 is measured to hold in a busy chamber on the test devices:

- **It applies at once, without a restart** (round 11). mtld3d reads its
  configuration once, at initialization, so Metal starts with
  `present.maxFps=<the screen's maximum>` (120 on ProMotion, 60 otherwise;
  `rt_ios_metal_launch_cap`), and a lower cap is paced by the app after each
  Present, the guest lock released while it waits (`metal_ios.m`,
  `rt_ios_pace` in `settings.c`). OpenGL: 60 and 120 are paced by the
  display; under 60 the runtime waits before each present (`gl_ios.m`,
  `limit_frame_rate`).
- Display: on a ProMotion screen a display link always asks for the cap
  itself (min = max = preferred; `rt_ios_display_range`: Auto asks for 120
  on a 120 Hz screen and 60 on a 60 Hz one) and follows every change; 40 asks
  for 120 and lands on every third refresh (on a 60 Hz panel 40 fps would
  alternate one and two refreshes; on a phone without ProMotion choose 30).

Auto power saving (on by default, `P2_IOS_POWER_SAVE=0` turns it off) lowers
the cap to 30 when Low Power Mode is on or the thermal state is serious or
critical, and logs `[ios] économie d'énergie (<reason>) : 30 i/s au lieu de
<cap>`; it applies at once too, on both renderers.

**FPS counter** (gear menu > Display > FPS counter, off by default, shared by
every game): a small readout with the renderer, frames per second over the
last half second, and the display's current refresh rate as the system
reports it (the display link's interval) with the cap the game asks for:
`120 HZ CAP 120`. It sits at the top centre in Portal's layout (top left in
Half-Life 2's), checked clear of every button at any Button size
(`tests/checks_touch_layout.c`); a tap on it hides it.

Simulator: `xcrun simctl spawn <device> defaults write <bundle id>
P2ResolutionScale -string 1.5`. `tests/checks_ios_settings.c` checks the
translation and that the keys match between the code and the settings page.

## Older phones

The app picks its Resolution, Upscaling (MetalFX) and Frames per second from
the device model and memory at **first launch only**, and only for the ones
the player never set (`runtime/ios/device_defaults.c`; the marker
`P2DefaultsFor` in the app's preferences records it was done). The table and
thresholds are one file, **`ios/device_defaults.txt`**, copied into the app:
tune it there and rebuild. Checked by `tests/checks_device_defaults.c`.

| Tier | Devices | Resolution | MetalFX | Frames per second |
|---|---|---|---|---|
| weak | A13, A14 (iPhone SE 2, 12 family) | ×1 | Quality | 30 |
| a15 | iPhone 13 and 13 mini, SE 3, iPad mini 6 (A15, 4 GB, 60 Hz) | ×1.5 | Quality | 30 |
| a15pro | iPhone 13 Pro and Pro Max (ProMotion), 14 and 14 Plus (A15, 6 GB) | ×1.5 | Quality | 40 on ProMotion, else 30 |
| a16 | iPhone 14 Pro, 15, 15 Plus; **any model not in the table** | ×2 | Quality | 60 |
| strong | A17 Pro and newer (15 Pro, 16, 17, 18 families, iPad mini A17 Pro and A20 Pro) | ×2 | Off | 60 |

The values are conservative guesses: nothing below an A17 Pro has been
measured. Auto (120 Hz) is nowhere a default until a device holds ~90 fps in
a busy chamber (round 1 rule).

- **In the game:** pause > gear > Display shows "Defaults for <device>"
  and a **Use recommended** button that applies them again (the frame rate
  at once with OpenGL; Resolution and MetalFX on the next launch). Reset to
  defaults goes back to these values too.
- **Low-memory guard** (`lowmem` in the file: 4,600 MB as iOS reports it,
  so every 4 GB device): the game also starts with `+mat_picmip 1` (textures
  at half resolution) and lower mtld3d memory caps. `P2_NO_LOWMEM=1` turns it
  off.
- **Memory log:** at each level load the log (Log setting, `game.log`) gets
  `[ios] mémoire (niveau chargé) : empreinte N Mo, pic N Mo, encore N Mo avant
  la limite d'iOS (<model>, <RAM> Mo)`, so a crash can be diagnosed later.
  The launch line `[ios] appareil : …` says which tier was picked.
- Simulator: no defaults are written there (it stays at ×1, OpenGL), unless
  `P2_DEVICE_MODEL=iPhone14,5` (and `P2_DEVICE_RAM_MB=3700` for the
  low-memory guard) asks for a phone's defaults on purpose.

## Gyro aiming

Turning the phone turns the camera, added to the touch camera: the thumb for
large turns, the wrists for aiming (`runtime/ios/gyro_aim.c`, tested by
`tests/checks_gyro_aim.c`; CoreMotion in `touch_controls.m`). Yaw follows the
real vertical, so turning on the spot turns the view however the phone is
tilted; pitch follows the screen's horizontal axis (top of the screen toward
you: look up). Rotations slower than `P2_GYRO_TIGHTEN` (0.05 rad/s) are damped
in proportion, so a phone held still does not drift. At Normal speed the camera
turns 1.5 times the phone's rotation at the game's default mouse sensitivity
(3); the game's `sensitivity` scales it like the mouse. The sensors stop after
2 s in a menu or with the overlay hidden for a controller, and at once when
the app is left.

On by default in Half-Life 2. Off in Portal 2, where each back tap would shake
the camera; `P2_GYRO=1` or the setting turns it on there too.

## Leaving the app

When the app stops being active (app switcher, home gesture, Control Center
or Notification Center pulled down, incoming call, screen locked), the game
pauses at its next frame (`runtime/ios/lifecycle_ios.m`): the main thread,
which is the game's, stops in `SDL_PollEvent`, finishes pending OpenGL ES
work (`glFinish`; iOS kills an app that uses the GPU through OpenGL ES in the
background, and refuses Metal submissions), suspends and detaches the OpenAL
context, then waits, serving UIKit's run loop, until the app is active again
(`UIApplicationDidBecomeActive`). Other game threads keep running (the guest
lock is released while it waits). The touch overlay treats the pause like a
menu (held fingers released, no input sent) and the back-tap sensors stop at
once. A frame presented by another thread while in the background is
dropped without any OpenGL call. Log lines: `[ios] app inactive : jeu en
pause`, `[ios] app en arrière-plan`, `[ios] app active : reprise après N s`.

| Variable | Default | Purpose |
|---|---|---|
| `P2_IOS_BACKGROUND_PAUSE` | 1 | `0`: no pause at all (the old behavior) |
| `P2_IOS_BACKGROUND_AUDIO` | 1 | `0`: the pause leaves the OpenAL context alone |

To check on the phone (with the Log setting on): in game, open the app
switcher and come back; go to the home screen, wait 30 s, reopen; pull down
Control Center over the game, then dismiss it; lock the screen and unlock.
Each time the game must come back where it was (no restart: a restart means
iOS killed it), with sound, and `game.log` must show the pause and resume
lines. Also try during a level load, and with the OpenGL renderer once.

## Half-Life 2

The same checkout builds Half-Life 2's iPhone app (see [hl2.md](hl2.md)):
`tools/build_ios.py --game hl2` reads the game profile (`tools/games.py`) for
the lift roots, the program (`hl2_linux`), the build paths (`build/hl2`, app
in `build/hl2/ios/<target>/HalfLife2.app`), the app name and icon, and writes
`P2Game` = `hl2` into the Info.plist. At launch
`runtime/ios/game_ios.c` reads that key (nothing is guessed): saves in
`Documents/hl2-userdata`, `SteamAppId` 220 for the Steam stub, `-game
hl2_complete`, back taps off, the HL2 touch layout.

```sh
./setup.sh --game hl2 --ios                # Mac build, then the iPhone app (--ios alone: every game)
# or by hand, as for Portal 2:
python3 tools/build_ios.py --game hl2 --target device --release --deps deps/ios-deps/device
tools/ios_device.sh --game hl2 install && tools/ios_device.sh --game hl2 data   # ~10 GB: bin platform hl2 episodic ep2 lostcoast hl2_complete
tools/run_ios_sim.sh --game hl2 120        # simulator, headless: build/hl2/ios/sim/game.log
```

The app identifier must differ from Portal 2's, or installing HL2 replaces
Portal 2 on the phone: `setup.sh` derives `local.hl2recomp.<team>` (Portal 2
keeps `local.portal2recomp.<team>`), stored per game: Portal 2's
`ios_bundle_id` at the top level of `config/local.json`, HL2's in
`games.hl2.ios_bundle_id` (`tools/ios_bundle_id.py --game hl2` prints it;
`P2_BUNDLE_ID` overrides both). It needs its own provisioning profile (first
`setup.sh --ios` run, or the `xcodebuild` command above with the HL2
identifier). With a free account the two apps take 2 of the 3 sideloaded-app
slots, and both need `./setup.sh --ios` again every 7 days. The name on the
home screen is "Half-Life 2".

Settings page: the same as Portal 2 without the Portals group.

**The fork's features in Half-Life 2's app** (round 8, built and checked on
the Mac and compiled for iOS; not run yet, see below):

- the **gear and settings menu** in the main and pause menus: Half-Life 2's
  engine is Source 2013 (`VEngineClient014`: IsInGame 26,
  IsLevelMainMenuBackground 87, ClientCmd_Unrestricted 106, read from the
  Mac edition's `engine.dylib`, checked at run time like Portal 2's), so the
  menu, its live settings, the version line naming the game and **Edit
  button layout** (HL2's own buttons) work there; no back-tap, portal-button
  or firing-vibration items (`rt_menu_item_for_game`), gyro aiming shown on
  by default as it is;
- the **older-phone defaults** (`ios/device_defaults.txt`), shared by every app;
- **iCloud saves** in Half-Life 2's own folder (`Documents/hl2/SAVE` in its
  app's container), once its app has the iCloud capability (its own App ID
  and container: the same one-time setup as Portal 2's, with the HL2 identifier).

Half-Life 2 stays its own app. Building and running it needs its Linux
binaries: `download_depot 220 224` in the Steam console (depot 224, "linux"
in Steam's own app info; 337 MB).

### Touch controls

Same style and behaviour as Portal 2 (hidden in menus and with a controller,
move stick on the left half, camera stick or trackpad on the right half; the
camera stick's resting ring sits nearer the centre, clear of the fire cluster), with
HL2's commands (`runtime/ios/touch_layout.c`):

| Where | Button | Key | Notes |
|---|---|---|---|
| bottom right, largest | fire | left click | dragging the finger while firing also aims (trackpad speed) |
| left of fire | alt-fire | right click | aims while dragged too |
| above fire | jump, reload | Space, R | |
| below fire | crouch | Ctrl | toggle: tap to crouch, tap again to stand; jump while it is lit to crouch-jump |
| bottom, left of crouch | use | E | |
| left, above the stick | sprint | Shift | toggle; stops when the thumb leaves the move stick |
| top centre, small | previous / next weapon | mouse wheel | the game's weapon list opens; fire picks the weapon (lines `+hud_fastswitch` and `1` in `Documents/launch.args` switch at once) |
| top right, small | flashlight, quick save, pause | F, F6, Escape | hold quick save for half a second to quick load (F9), with a vibration |
| top left, after the FPS counter, small | camera mode | | stick or trackpad, as in Portal 2 (moved off the HUD's health) |

Gyro aiming is on: turning the phone turns the camera, on top of the stick or
trackpad (see [Gyro aiming](#gyro-aiming)).

Why toggles: on a phone each thumb is already busy (one on the move stick, one
on the camera or fire), so a held Shift or Ctrl would stop you from steering or
aiming; crawling through vents or sprinting down a corridor needs both.

Back taps are off: they produce a 50 ms click, while HL2's automatic weapons
fire as long as the button is held, and an accidental shot wastes ammo or
alerts enemies. `P2_BACKTAP=1` turns them back on (left tap: fire, right tap:
alt-fire); the Settings page has no back-tap entries for HL2.

### Status

Checked on an iPhone 15 Pro Max (8 Nov. 2026, short session): install and
signing with the HL2 identifier, data copy (~10 GB, about 5 minutes over the
cable), Metal, 60 fps, moving, firing, flashlight. Earlier, the iOS Simulator
(software rendering, 2–6 fps) checked the menu, the stick, aiming, the weapon
list and quick save.

Not verified yet: memory and thermal behaviour, a long session, sound,
`setup.sh --ios` end to end. The ammo readout at the bottom right sits partly
under the crouch and use buttons.

HL2-specific iOS pieces: the Steam stub answers HL2's SDK 1.60 start-up
(`SteamInternal_SteamAPI_Init`) and the interfaces it calls by vtable slot
(`tools/gen_steam_stub.py` generates a table per version); the minimal
fontconfig accepts the game's own fonts (`FcConfigAppFontAddFile`); HL2
always gets `-w`/`-h` at the render size (it would start at 640×480).
Steam Input reloads its action manifest every few seconds in game (one small
file read; harmless, not fixed).

## Portal

The same checkout also builds Portal's iPhone app (see
[portal1.md](portal1.md)): `tools/build_ios.py --game portal1` reads the game
profile (`tools/games.py`), writes `P2Game` = `portal1` into the Info.plist
and builds `build/portal1/ios/<target>/Portal.app`. At launch
`runtime/ios/game_ios.c` reads that key: saves in `Documents/portal-userdata`,
`SteamAppId` 400 for the Steam stub, `-game portal`, back taps on (they fire
the portal) and **Portal 2's touch layout**, since it already has both portal
buttons.

```sh
./setup.sh --game portal1 --ios            # Mac build, then the iPhone app
# or by hand, as for Portal 2:
python3 tools/build_ios.py --game portal1 --target device --release --deps deps/ios-deps/device
tools/ios_device.sh --game portal1 install && tools/ios_device.sh --game portal1 data   # ~4 GB: platform portal hl2
tools/run_ios_sim.sh --game portal1 120    # simulator, headless
```

The identifier is `local.portal1recomp.<team>`, so it sits next to Portal 2's
and Half-Life 2's apps (`games.portal1.ios_bundle_id` in
`config/local.json`; `tools/ios_bundle_id.py --game portal1` prints it). The
name on the home screen is "Portal". The Settings page is Portal 2's: the
Portals group (back taps, portal buttons, vibration) and gyro aiming off by
default, as for Portal 2. `tools/ios_device.sh saves` is still Portal 2 only,
so Portal's saves are copied by hand for now.

Status (12 November 2026): installs, starts and renders on an iPhone 17 Pro
Max (short session, menu and touch layout; no chamber played). The devices
were locked for the last run of this checkout, so the app was not exercised
again. Same experimental status as the Mac app
([portal1.md](portal1.md#what-was-tested)).

## A phone as a controller (fork, round 23)

Gear menu > **Phone controller** shows a QR code (on the iPad, and on the TV
when one is connected) and a 4-digit code. A friend scans it with the phone's
camera, on the same Wi-Fi, and gets a page with Portal 2's controls: a stick
under each thumb, both portals, jump, crouch, use, pause. Nothing to install,
on iPhone or Android. Share > **Add to Home Screen** (Android: Install app)
keeps it as a full-screen landscape app.

- **Who it controls:** the phone is a game controller plugged in after any
  real one: player 2 in co-op with a controller (or the iPad's touch) as
  player 1, player 1 when nothing else plays. **One phone at a time:**
  Portal 2's co-op has two players, so a second phone is told "busy".
- **Safety:** the server runs only once the panel has been opened, on Wi-Fi
  and wired networks (never cellular), answers local addresses only, and
  wants this session's code. **Kick phone** disconnects it and makes a new
  code (the page asks for it).
- **Addresses:** the QR code carries the iPad's IP (any phone resolves it);
  the page then moves to `sauce-XXXX.local`, a name Sauce registers with
  Bonjour, so a saved home-screen app keeps working when the iPad's IP
  changes. A phone that can't resolve `.local` stays on the IP.
- **Input lag** (measured from a Mac on the same network, `tools/phonepad_probe.py`):
  3.8 ms round trip at the median, about 110 ms at the 95th percentile (Wi-Fi
  power-saving bursts). The page shows its own round trip; Sauce logs it.
- **Closing the panel** (X, Done, or a controller's A or B) never disconnects
  the phone. With no phone connected, it stops pairing.
- **Nothing stays held:** fire, buttons and sticks are released on release, and
  all at once when the phone goes quiet for `P2_PHONEPAD_QUIET` seconds (2.5),
  hides the page, or Sauce goes inactive.
- Code: `runtime/ios/phonepad.c` (rules, checked), `phonepad_ios.m`
  (server, Bonjour name, QR panel, the virtual controller), `ios/phonepad/`
  (the page). Port `P2_PHONEPAD_PORT` (8017).

## Online co-op on iPad and iPhone (fork, round 24)

Portal 2's **Standard** and **Challenge Mode co-op** find a partner through
Steam lobbies, and Steam doesn't run on iOS. After `P2_ONLINE_TIMEOUT`
seconds (8) on "Searching for Friends...", Sauce explains this and points to
Local Splitscreen. Its **Back** gives the game Steam's own "no connection"
answer, and the game closes the search itself ("Failed to create session",
OK). Two iOS devices can't find each other yet. The only network co-op is
still a Mac host with `connect <address>` (see docs/usage.md).

## Sauce: one app for every game (fork, round 19)

Since round 19 every game lives in **one app, Sauce** (its home-screen name;
`APP_NAME` in `tools/games.py`, or `"ios_app_name"` in `config/local.json`,
renames it). It is the Portal app underneath: same bundle ID, provisioning
profile and increased-memory entitlement, so saves, settings and iCloud carry
over; only the name and the icon (our own art, `tools/make_app_icon.swift`)
changed. The game picker offers Portal 2, Portal Stories: Mel, Portal,
Half-Life 2 and Left 4 Dead 2; each keeps its data in `Documents/<game>`
(Portal 2's at the root, as before), its saves in `Documents/<userdata>`, its
launch arguments (`launch-<game>.args`, L4D2's offline ones included) and its
iCloud folder. The settings menu's last line reads "Sauce <version> · <game>".

- **One binary.** Each game's generated code is linked into the one executable
  (as Mel's was in round 6), reached at launch through `rt_code`; a game whose
  program isn't Portal 2's names it in its launch file (`--program hl2_linux`).
  The executable is about 1.7 GB, the app 1.6 GB on disk (round 19).
- **Data:** `tools/ios_device.sh --game hl2|l4d2|portal1 data` copies a game into
  Sauce; `./setup.sh --ios` builds Sauce once and copies every selected game.
- **From the former Half-Life 2 and Left 4 Dead 2 apps:**
  `tools/ios_device.sh --game hl2|l4d2 migrate [--dry-run]` copies the game's
  saves folder and the old app's preferences into Sauce through the Mac,
  checks every file, and deletes nothing. At the game's next launch its old
  button layout fills in the buttons the shared layout doesn't place yet. Then
  remove the old app by hand.
- **First launch of a game is slower:** the shader cache starts empty in a new
  app, so the first minutes of a game compile shaders (Half-Life 2 and L4D2
  dipped to 75–95 fps on the iPad Pro); from the second launch they hold their
  usual rate (120 fps).

## On a TV (fork, round 20)

Plug the iPad into a TV (an HDMI adapter on USB-C), or AirPlay to an Apple TV
from Control Center > Screen Mirroring: the game draws its own picture on the
TV, at the TV's shape (16:9, no black bars) and resolution, and the iPad
becomes the pad (the touch controls over black; with a controller in use, a
dim "Playing on TV"). Unplug, or stop AirPlay, and the picture comes back to
the iPad, mid-game. Every game in Sauce does this (`runtime/ios/tv_output*.{c,m}`).

- **Gear menu > TV** (and the Settings app, shared by every game): Picture on
  the TV (on), TV resolution (Auto: the TV's own, at most 4K; 1080p; 1440p;
  4K) and TV frame rate (the TV's rate, usually 60; 30; 60; 120).
- **How:** the game's Metal view moves into a window on the external screen,
  its drawable takes the TV's size, and the engine switches its picture to that
  size (`mat_setvideomode W H 1`). 4K needs a cable and a 4K TV; AirPlay usually
  gives 1080p.
- **Tests without a TV:** `P2_TV_SIMULATE=1920x1080` makes a 16:9 band across
  the iPad's top stand in for the TV; `P2_TV_SIMULATE_CYCLE=30` unplugs and
  plugs it back every 30 seconds.
- **Measured on the M5 iPad Pro, simulated 4K:** Left 4 Dead 2 `c1m1_hotel` at
  120 fps, 2.4 GB; Half-Life 2 at its 60 fps cap. A real TV at 60 Hz caps the
  game at 60.

## One Portal app: Portal 2 and Mel

Portal 2 and Portal Stories: Mel (and later any game on Portal 2's engine,
`ios_guests` in `tools/games.py`) live in **one app**: Portal 2's, with its
identifier, profile and increased memory limit, which Mel now shares.
Half-Life 2 and Portal stay separate apps.

- **One executable, two games.** `tools/build_ios.py` compiles each hosted
  game's generated code in its own cache and links it into one relocatable
  object (`ld -r`) whose only exports are six `<game>_rt_...` functions
  (`runtime/ios/guest_glue.c`); its ~135,000 `fn_<address>` functions stay
  local, so they never meet Portal 2's. The runtime is built once with
  `RT_MULTI_GAME` and reaches the running game's code through `rt_code`
  (`runtime/rt.h`, table generated as `build/ios/<target>/game_codes.c`).
  Only one game runs per launch. The executable is 686 MB (Portal 2 alone:
  315 MB). `--no-guests` builds Portal 2 alone.
- **Game picker** at launch (`runtime/ios/game_picker_ios.m`, rules in
  `game_picker.c`): each game's cover (made at build time from the game's own
  icon on the Mac, `covers/<game>.png`), title and size; "Not installed" when
  its files aren't on the device. **Remember my choice** skips it next time;
  **Switch game** at the bottom of the in-game settings menu brings it back
  (press twice: the app closes, open it again).
- **Manage games** (in the picker): data size, saves size and last played
  for each game; **Remove game data** deletes that game's `game`, `prep` and
  `linux` folders only (never its saves or settings, never the other game's),
  after a confirmation that says how much it frees; **Get it from your Mac**
  says how. **Portal 2 has no Remove**: in its place, one line says it's the
  base the other games run on (Mel borrows its files), and the Mac
  installer's Devices section never offers to remove it either.
- **Each game keeps its own things.** Portal 2's data, saves, settings and
  `launch.args` stay exactly where they were (`Documents/game`, `prep`,
  `linux`, `portal2-userdata`, the Settings app page). Mel's data is in
  `Documents/mel/` (`game`, `prep`, `linux`, `launch.args`), its saves in
  `Documents/mel-userdata`. iCloud saves sync Mel in its own folder (`Documents/mel/SAVE` in the
  container). Community Maps and Maps & Mods stay Portal 2 only. The menu's
  version line names the game.
- **One set of controls and settings for every game** (round 11,
  `rt_ios_pref_key` in `runtime/ios/game_ios.c`): the touch layout from the
  layout editor, button size and opacity, back taps (switch, sensitivity,
  calibration), gyro and camera speed, frame rate, the FPS counter and every
  other general setting of the gear menu are the same keys in every game, so
  a change in one applies in the other (and in the Settings app page). Only
  what means something to one game stays per game: its iCloud saves switch
  and last sync (`mel.`-prefixed keys), and by their folders its saves,
  `launch.args` and the version line. Saving the layout from one game keeps
  the place of a button only another game has (`rt_touch_custom_merge`).
  Migration: Portal 2's settings became the shared set; Mel's old `mel.*`
  keys stay in the preferences, untouched, and are copied once to
  `Documents/mel-userdata/settings-before-shared.plist`.
- **Mel's slim pack** (round 11, `tools/mel_slim.py`): Mel ships its own
  copy of Portal 2's `portal2` folder (2.4 GB, almost all of it VPK). Of its
  14,977 VPK entries, 14,656 are byte for byte in Portal 2's own VPK. The slim
  pack keeps Mel's VPK *directory* as `portal2/pak01_slim.vpk` (version 1, as
  the engine reads): Mel's entries exactly, each pointing into Portal 2's
  archives, or carrying its bytes when Portal 2's differ (321 entries,
  27 MB). With the marker `Documents/mel/.p2-slim`, `ios/main_ios.c` sets
  `P2_BASE_GAME_ROOT=Documents/game` and the runtime (`runtime/paths.c`)
  gives the engine that directory for `portal2/pak01_dir.vpk` and Portal 2's
  archives for `portal2/pak01_NNN.vpk`; every loose file stays Mel's. Mel
  then sees the same entries with the same bytes as with its full copy, and
  none of Portal 2's (Source searches every VPK before any loose file).
  9.18 GB becomes 6.83 GB (2.36 GB freed). Without Portal 2's files the
  picker says "Needs Portal 2" with a sentence, and the runtime refuses to
  start with the same sentence (no crash). Checked by `tests/test_mel_slim.py`
  (`P2_REAL_MEL_SLIM=1` on the real games: every entry, same bytes) and on
  the Mac with the device's layout (`mel_slim.py layout`): a save made and
  loaded, the console identical to the full copy's.
- **Tools:** `tools/ios_device.sh mel data push` copies Mel into the Portal
  app (`Documents/mel`), as the slim pack (`--full` for the whole copy;
  Portal 2 must be on the device first); `tools/ios_device.sh mel slim switch`
  turns a full copy slim without ever losing it: the slim directory, a boot
  test with it (a save or the first map, checked in the console), and only
  then the app removes Mel's own copies of Portal 2's archives; `tools/ios_device.sh mel saves migrate [--dry-run]`
  copies Mel's saves from the old standalone Mel app through the Mac and checks
  every file arrived (the old app is not touched). `run`, `install` and the
  simulator script start the game asked for (`P2_GAME_CHOICE`); an empty
  `SIMCTL_CHILD_P2_GAME_CHOICE` and `P2_PICKER_TEST=<game>` show the picker
  and pick that game after six seconds (screenshots).
- **From the Mac without Terminal:** the Natif Installer's **Devices** step
  (`tools/devices.py`, docs/usage.md) lists the iPhones and iPads plugged in or
  paired on the Wi-Fi, their free space and games, and downloads Portal 2,
  Mel or the Mac's Workshop maps, installing the app first when needed.

## Portal Stories: Mel (phase 1)

Mel now lives in the Portal app (above); a standalone Mel app can still be
built (`P2_STANDALONE=1` for the device tools). The same checkout builds Mel's own app ([mel.md](mel.md), plan in
[fork/MEL_IOS_PLAN.md](fork/MEL_IOS_PLAN.md)): `tools/build_ios.py --game mel`
writes `P2Game` = `mel` and builds `build/mel/ios/<target>/PortalStoriesMel.app`
("Portal Stories Mel" on the home screen). At launch `runtime/ios/game_ios.c`
gives `-game portal_stories`, saves in `Documents/mel-userdata`, `SteamAppId`
317400 for the Steam stub (Mel's SDK 1.40 interfaces are in its tables),
Portal 2's touch layout and back taps. Mel's 2021 engine uses Portal 2's
interface versions, so the pause-menu gear and settings menu work; Maps & Mods,
Community Maps and iCloud saves are Portal 2 only.

```sh
python3 tools/build_ios.py --game mel --target device --release --deps deps/ios-deps/device
P2_DEVICE=<UDID> tools/ios_device.sh --game mel install
P2_DEVICE=<UDID> tools/ios_device.sh mel data push --dry-run   # what goes, ~9.3 GB
P2_DEVICE=<UDID> tools/ios_device.sh mel data push             # checks free space first
tools/run_ios_sim.sh --game mel 300                            # simulator, headless
```

- **Identifier** `local.melrecomp.<team>` (`games.mel.ios_bundle_id`). It is
  signed with the team's wildcard development profile (`ios/signing/sign.py`
  falls back to it), so no App ID is registered for it. That profile has no
  increased memory limit: fine on 8 GB and larger devices, not enough for a
  4 GB phone (see the plan).
- **Data:** `mel data push` copies the game folders, the Linux depot and the
  prepared modules (Mel's i386 `libstdc++` and `libgcc_s` among them) and
  leaves the Mac player's `SAVE`, screenshots and logs behind
  (`tools/ios_data_plan.py`).

## Diagnostics

iOS-specific variables; `P2_FRAME_LOG`, `P2_GATE_TIMING`, `P2_TRACE_*`,
`P2_NATIVE_<NAME>=0`, `P2_GL_STREAM_UNSYNC=0` and the other runtime variables
work as on the Mac ([usage.md](usage.md#diagnostics)). The `P2_IOS_GL_TRACE`,
`P2_IOS_DEBUG_UV`, `P2_IOS_SHADER_DUMP`, `P2_IOS_SRGB_TRACE` and
`P2_IOS_SKIP_DRAWS` family needs a debug build (`P2_IOS_GL_DEBUG=1` when
building with `tools/build_ios.py`). `P2_TOUCH_TEST_*` is for the simulator
(no finger on the screen).

| Variable | Effect |
|---|---|
| `P2_LOG_FILE=Documents/game.log` | log file (relative to the container) |
| `P2_PROFILE_DELAY=60`, `P2_PROFILE_RAW=1` | built-in profiler (with `P2_FRAME_LOG=1`); with `RAW`, `tools/ios_symbolize.py` resolves driver functions using the device's symbols (Xcode, iOS DeviceSupport) |
| `P2_TOUCH_CONTROLS=0/1/pad` | touch controls disabled / always shown / off while a controller is connected |
| + `_LOW=300`, `_HIGH=1500`, `_SMOOTH=0.5`, `_LOG=1` | trackpad acceleration: gain of 1 below `_LOW` pt/s, ramping up to the `P2_TOUCH_ACCEL` multiplier at `_HIGH` pt/s; speed is smoothed (weight `_SMOOTH`); `=1` disables it, `_LOG` logs speed and gain |
| `P2_BACKTAP=0` | disables back taps (enabled by default if the sensor exists; see "Back taps") |
| `P2_BACKTAP_LOG=1` | logs every candidate (from half the thresholds) and the background noise every 5 s |
| `P2_TOUCH_TEST_CLICK=s,x,y` | simulator: left click at (x, y) points, s seconds after the first menu |
| `P2_TOUCH_TEST_MOVE=f,s` + `_SECONDS`, `_DELAY`, `_ANGLE`, `P2_TOUCH_TEST_WASD` | simulator: forced deflections of the movement stick |
| `P2_IOS_RENDER_SCALE=1/2/native` | render scale (points → pixels); `tools/ios_bench.py` compares antialiasing × scale (`--renderer opengl` by default, `--renderer d3d9` for Metal) |
| `P2_IOS_NO_PRESENT=1` | frames rendered but never presented, so the display cap does not limit the measured rate (OpenGL only; Metal holds presents to `P2_IOS_MAX_FPS`) |
| `P2_IOS_AUTOEXEC="cmd; convar=value; pause 5; …"` | console commands, convar settings and pauses (seconds) run in order once the menu is gone, starting 5 s later |
| `P2_IOS_BGRA=0`, `P2_IOS_SRGB=0` | OpenGL: disable the BGRA fast path or the sRGB emulation |
| `P2_IOS_SHADER_DUMP=<directory>` | debug build: converted shaders |
| `P2_IOS_DRAW_LOG=<frame>` | draws for one frame |
| `P2_IOS_GL_TRACE=1` | debug build: logs each distinct texture upload combination once (function, target, level, formats, GL error) and traces shader compilation and program linking |
| `P2_IOS_PROBE_FRAMES=1` | every 120 frames, reads a few pixels of the system framebuffer before presenting (all black = nothing drawn) |
| `P2_IOS_DEBUG_UV=1` | debug build: every fragment shader that receives `oT0` shows `fract(oT0.xy)` instead (texture coordinate check) |
| `P2_IOS_SKIP_DRAWS=<n>` or `<a>-<b>` | debug build: skips draw n (or a up to b, excluded) of every frame, counted as in `P2_IOS_DRAW_LOG` |
| `P2_IOS_SKIP_PROGRAMS=45,802` | skips every draw made with these GL program ids |
| `P2_IOS_PROGRAM_DUMP=<directory>` | writes the shader sources of the programs described by `P2_IOS_DRAW_LOG` |
| `P2_IOS_SRGB_TRACE=1` | debug build: sRGB and border-state statistics every 2 000 draws |
| `P2_FONT_DIRS=<dir>:<dir>` | font directories indexed by the minimal fontconfig (default `/System/Library/Fonts`) |
| `P2_FONT_LOG=1` | logs every font match (by default only the first request per family) |

Game console: `-condebug` writes it to
`Documents/portal2-userdata/portal2/console.log` (Half-Life 2: under
`Documents/hl2-userdata/`).

## Back taps

Holding the phone in landscape, tapping the back near the left edge with an
index finger fires the blue portal, near the right edge the orange one
(`runtime/ios/back_tap.c`, tested by `tests/checks_back_tap.c`; CoreMotion in
`touch_controls.m`, on a dedicated queue, no permission required). Back taps
replace the overlay's portal buttons, which only come back without a motion
sensor (simulator) or with `P2_BACKTAP=0`.

Settings > Portal 2 has the back-tap entries (on/off, sensitivity, swap left
and right, record taps). How the detector works, its variables, raw
recordings and the tap dataset: [backtap.md](backtap.md).

## Performance

60 fps by default, steady on an iPhone 15 Pro Max; `P2_IOS_MAX_FPS=120`
requests 120 Hz (the frame rate then lands between 60 and 120, since the
ProMotion display shows a frame taking more than 8.3 ms at 60). The GPU is the
bottleneck, so the render scale is ×2 by default. `tools/ios_bench.py`
measures a scene (`--renderer d3d9` for Metal). Metal against OpenGL and the
OpenGL optimisation history: [STATUS.md](../STATUS.md#measurements).

## iOS-specific pitfalls

- The ES linker rejects a program whose fragment shader reads an output
  that the vertex shader does not write (desktop GL accepts it):
  the skipped draws broke the view through portals and froze the
  Options menu. `essl3.c` initializes these outputs.

- Case-sensitive file system: the engine writes and reads in
  lowercase (`portal2/save/…`); `runtime/paths.c` and
  `runtime/ios/case_path.c` recover the actual case.
- SDL never sees the game "capture" the mouse: the menu/gameplay state comes from the
  engine (`IEngineVGui::IsGameUIVisible`, `runtime/ios/game_state.c`).
- Touch controls: a UIKit view placed on top of the window, made of fixed
  layers (one `CAShapeLayer` per stick and button, a small bitmap for the FPS
  counter) moved without animation and flushed once per run-loop pass (SDL's
  loop never commits UIKit transactions).

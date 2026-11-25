"""Per-game layout of the Linux depot: which modules to prepare and how the
program is named, plus what setup.sh and tools/make_app.py need to install it
(Steam depot, Steamworks.NET release, app name, data folder), and where its
build lives in this checkout. One checkout builds every game side by side:
Portal 2 keeps its historical paths (build/game, build/portal2-native,
build/ios), Half-Life 2 lives under build/hl2, Portal 1 under build/portal1.

Which game a tool works on: its --game option, else the P2_GAME environment
variable, else "game" in config/local.json, else the only game configured
there, else portal2 (selected()).
"""
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# The one iPhone and iPad app every game lives in (round 19): its name on the
# home screen. A rename is this line, or "ios_app_name" in config/local.json.
# No Valve trademark here or in its icon (ios/AppIcon.png); game names stay
# inside the game picker.
APP_NAME = 'Sauce'


def app_name(config=None):
    """The one app's display name: config/local.json's ios_app_name, else APP_NAME."""
    return (config or {}).get('ios_app_name') or APP_NAME

# Modules never prepared, whatever the game: authoring tools, the optional
# legacy Steam client and DXVK (shaderapivk.so gets its Direct3D 9 from the
# runtime bridge, runtime/d3d9.c, so Direct3DCreate9 becomes a shim).
_COMMON_EXCLUDED = {'bsppack.so', 'crashhandler.so', 'dedicated.so', 'vrad_dll.so', 'vvis_dll.so',
                    'libsteam.so', 'libdxvk_d3d9.so'}

GAMES = {
    'portal2': dict(
        program='portal2_linux',
        lib_dirs=('bin/linux32', 'portal2/bin/linux32'),
        mod='portal2',
        excluded=_COMMON_EXCLUDED | {'puzzlemaker_dll.so'},
        roots='config/game-roots.json',
        steam_appid=620,
        depot_id=661,
        # Installed app and its data: unchanged since the first release, so
        # existing installs, saves and settings stay where they are.
        app_name='Portal 2 Natif',
        log_name='Portal2Natif.log',
        bundle_id='fr.kanapitsas.portal2-native',
        icon='portal2/resource/game.icns',
        renderer=None,   # Metal when libd3d9metal is there
        # Build tree (prep, gen, obj, launch.log), executable and its name in
        # the app, iPhone builds: the paths of the single-game checkouts.
        build_dir='build/game',
        binary='build/portal2-native',
        executable='portal2-native',
        ios_dir='build/ios',
        ios=True,
        # iOS app (tools/build_ios.py): home screen name, executable, bundle
        # folder name; the Info.plist key P2Game (runtime/ios/game_ios.c) is
        # the profile's name.
        ios_name='Portal 2',
        ios_executable='portal2',
        ios_app='Portal2.app',   # in ios_dir/<target>
        # Folders of the game copy that tools/ios_device.sh data copies to the
        # device (Documents/game), without the soundtrack or sdk_content.
        ios_data=('bin', 'platform', 'update', 'portal2', 'portal2_dlc1', 'portal2_dlc2'),
        # Every other game lives in this app too, the one app (games.APP_NAME,
        # round 19; game picker at launch, runtime/ios/game_picker_ios.m): Mel on
        # its own Portal 2 engine, Portal and Half-Life 2 on Source 2013, Left 4
        # Dead 2 on its own branch. Each has its data in
        # Documents/<name> and its own saves and settings: one app ID, one
        # profile, Portal 2's increased memory limit for all of them.
        ios_guests=('mel', 'portal1', 'hl2', 'l4d2'),
    ),
    # Half-Life 2 since the 20th anniversary update (depot 224): episodes and
    # Lost Coast live in the hl2_complete mod; hl2/ and hl2mp/ hold other
    # client.so/server.so with the same names, so only hl2_complete is taken.
    'hl2': dict(
        program='hl2_linux',
        lib_dirs=('bin', 'hl2_complete/bin'),
        mod='hl2_complete',
        # Chromium (libcef, chromehtml and its ffmpeg) only serves in-game web
        # pages; vtex is a tool; libTelemetryX64 is a 64-bit profiler and
        # ffmpegsumo.so a leftover Mach-O bundle. Nothing loads libmimalloc.so
        # (Portal 2 preloads it; hl2.sh does not). The bundled FreeType,
        # Fontconfig and HarfBuzz would need the system zlib, bzip2 and expat:
        # the host ones are used through runtime/fonts.c, as for Portal 2.
        # sourcevr.so needs OpenVR and libstdc++, absent here; the engine
        # starts without it.
        excluded=_COMMON_EXCLUDED | {'vtex_dll.so', 'libcef.so', 'chromehtml.so', 'libffmpegsumo.so',
                                     'ffmpegsumo.so', 'libTelemetryX64.so', 'libmimalloc.so',
                                     'libfreetype.so', 'libfontconfig.so', 'libharfbuzz.so',
                                     'sourcevr.so'},
        roots='config/hl2-roots.json',
        steam_appid=220,
        depot_id=224,
        app_name='Half-Life 2 Natif',
        log_name='HalfLife2Natif.log',
        bundle_id='fr.kanapitsas.hl2-native',
        icon='hl2/resource/game.icns',
        renderer=None,   # Metal when libd3d9metal is there, as for Portal 2 (docs/hl2.md)
        build_dir='build/hl2',
        binary='build/hl2/hl2-native',
        executable='hl2-native',
        ios_dir='build/hl2/ios',
        ios=True,   # in the one app (portal2's ios_guests, round 19); a standalone build still works
        ios_host='portal2',
        ios_name='Half-Life 2',
        ios_executable='hl2',
        ios_app='HalfLife2.app',
        # hl2_complete mounts the VPKs of hl2, episodic and ep2 (gameinfo.txt);
        # lostcoast for its map. hl2mp and the macOS launcher are not needed.
        ios_data=('bin', 'platform', 'hl2', 'episodic', 'ep2', 'lostcoast', 'hl2_complete'),
    ),
    # Portal 1 (Steam app 400, data depot 401, Linux depot 404): the Source 2013
    # Linux build, the same generation as Half-Life 2's (shaderapivk/DXVK, CEF,
    # mimalloc kept out like there). Experimental: brought up from this checkout,
    # see docs/portal1.md.
    'portal1': dict(
        program='hl2_linux',
        # bin/ and portal/bin/ for the modules; the Miles plugins come from the
        # Linux depot (bin/linux32) like Half-Life 2's.
        lib_dirs=('bin', 'portal/bin'),
        mod='portal',
        excluded=_COMMON_EXCLUDED | {'vtex_dll.so', 'libcef.so', 'chromehtml.so', 'libffmpegsumo.so',
                                     'ffmpegsumo.so', 'libTelemetryX64.so', 'libTelemetryX86.so',
                                     'libmimalloc.so', 'libfreetype.so', 'libfontconfig.so', 'libharfbuzz.so',
                                     'sourcevr.so', 'libprofiler.so.0'},
        roots='config/portal1-roots.json',
        steam_appid=400,
        depot_id=404,
        app_name='Portal Natif',
        log_name='PortalNatif.log',
        bundle_id='fr.kanapitsas.portal-native',
        icon='portal/resource/game.icns',
        renderer=None,   # Metal when libd3d9metal is there, as for Portal 2
        build_dir='build/portal1',
        binary='build/portal1/portal1-native',
        executable='portal1-native',
        ios_dir='build/portal1/ios',
        ios=True,   # in the Portal app (portal2's ios_guests); a standalone build still works
        ios_host='portal2',
        ios_name='Portal',
        ios_executable='portal',
        ios_app='Portal.app',
        # bin/ is left out on purpose: Portal 1 is Source 2013, so libMiles takes
        # its plugins (mssmp3.asi, mssmixer.mix, binkawin.asi) from the Linux
        # depot (bin/linux32), which ios_device.sh data copies wholly as
        # Documents/linux and runtime/paths.c reads there as for Half-Life 2.
        # Sound, voices included, works on an iPhone whose data starts empty.
        ios_data=('platform', 'portal', 'hl2'),
    ),
    # Portal Stories: Mel (Steam app 317400): a Portal 2 mod shipped with its own
    # copy of the Portal 2 engine (2021 Linux build, bin/*.so, shaderapivk/DXVK
    # like Portal 2's), Linux binaries in depot 317404. Its game libraries
    # (portal_stories/bin) are in the shared data depot, hence game_lib_dirs,
    # relative to the game folder. Steamworks: SDK 1.40 interfaces (docs/mel.md).
    # Experimental, see docs/mel.md.
    'mel': dict(
        program='portal2_linux',
        lib_dirs=('bin',),
        game_lib_dirs=('portal_stories/bin',),
        system_libs=('libstdc++.so.6', 'libgcc_s.so.1'),
        mod='portal_stories',
        excluded=_COMMON_EXCLUDED | {'puzzlemaker_dll.so', 'libsteam_api_linux.so', 'libtcmalloc_minimal.so.4'},
        roots='config/mel-roots.json',
        steam_appid=317400,
        depot_id=317404,
        app_name='Portal Stories Mel Natif',
        log_name='PortalStoriesMelNatif.log',
        bundle_id='fr.kanapitsas.mel-native',
        icon='portal2.ico',
        renderer=None,   # Metal when libd3d9metal is there, as for Portal 2
        build_dir='build/mel',
        binary='build/mel/mel-native',
        executable='mel-native',
        ios_dir='build/mel/ios',
        ios=True,   # in the Portal app (portal2's ios_guests); a standalone build still works
        ios_host='portal2',
        ios_name='Portal Stories Mel',
        ios_executable='mel',
        ios_app='PortalStoriesMel.app',
        ios_data=('bin', 'platform', 'portal2', 'portal2_dlc2', 'portal_stories'),
    ),
    # Left 4 Dead 2 (Steam app 550, Linux binaries depot 554): i386 like Portal 2,
    # on the engine branch Portal 2 grew from (docs/fork/L4D2_RESEARCH.md). Its
    # modules link libstdc++ and libgcc_s dynamically, as Mel's do (system_libs,
    # Debian i386 packages). Local and offline only: launched with -insecure,
    # +sv_lan 1 and -nomaster (launch_args), never connected to Valve's servers.
    # Experimental, see docs/l4d2.md.
    'l4d2': dict(
        program='hl2_linux',
        lib_dirs=('bin', 'left4dead2/bin'),
        system_libs=('libstdc++.so.6', 'libgcc_s.so.1'),
        mod='left4dead2',
        # Chromium (in-game web pages: the store and news panels), the dx9
        # shader API (unused: shaderapivk goes through the runtime's Direct3D 9
        # bridge), the dedicated-server copies (*_srv.so, loaded by srcds only)
        # and the unit test library. libtogl.so stays: the launcher links it,
        # as Portal 2's does.
        excluded=_COMMON_EXCLUDED | {'libcef.so', 'chromehtml.so', 'shaderapidx9.so',
                                     'libsteamvalidateuseridtickets.so', 'unitlib.so', 'unitlib_srv.so',
                                     'datacache_srv.so', 'dedicated_srv.so', 'engine_srv.so', 'libtier0_srv.so',
                                     'libvstdlib_srv.so', 'materialsystem_srv.so', 'scenefilecache_srv.so',
                                     'shaderapiempty_srv.so', 'soundemittersystem_srv.so', 'studiorender_srv.so',
                                     'vphysics_srv.so', 'vscript_srv.so'},
        roots='config/l4d2-roots.json',
        # vstdlib's coroutines (used by matchmaking.so) switch stacks by copying
        # them and longjmp-ing into a frame that isn't on the host's call stack:
        # the runtime runs them instead, each on its own thread (runtime/coroutine.c).
        host_exports=('libvstdlib.so:Coroutine_',),
        steam_appid=550,
        depot_id=554,
        # Offline: the local listen server never asks Valve's master server or VAC.
        launch_args=('-insecure', '+sv_lan', '1', '-nomaster'),
        # And the Steam bridge refuses server lists, pings and lobbies (P2_STEAM_OFFLINE).
        steam_offline=True,
        app_name='Left 4 Dead 2 Natif',
        log_name='Left4Dead2Natif.log',
        bundle_id='fr.kanapitsas.l4d2-native',
        icon='left4dead2.ico',
        renderer=None,   # Metal when libd3d9metal is there, as for Portal 2
        build_dir='build/l4d2',
        binary='build/l4d2/l4d2-native',
        executable='l4d2-native',
        ios_dir='build/l4d2/ios',
        ios=True,    # in the one app since round 19 (its own app in round 15); standalone still builds
        ios_host='portal2',
        ios_name='Left 4 Dead 2',
        ios_executable='l4d2',
        ios_app='Left4Dead2.app',
        # hl2: its gameinfo's last search path, where the shaders are (without
        # it the device draws a black screen; missing_ios_data checks it).
        ios_data=('bin', 'platform', 'update', 'left4dead2', 'left4dead2_dlc1', 'left4dead2_dlc2',
                  'left4dead2_dlc3', 'hl2'),
    ),
}


DEFAULT = 'portal2'
# Executable names of every game: "is a recompiled game running" checks
# (uninstall.sh, tests) must cover them all.
EXECUTABLES = tuple(g['executable'] for g in GAMES.values())


def forced_args(game, args):
    """`args` with the profile's own arguments after them, always, whatever the
    launcher was given: Left 4 Dead 2's offline set (-insecure +sv_lan 1
    -nomaster), so a recompiled build never meets Valve's servers or VAC."""
    extra = list(game.get('launch_args', ()))
    return [*args, *extra] if extra else list(args)


def check(name, origin='--game'):
    """`name` if it is a known game, else a clear exit."""
    if name not in GAMES:
        raise SystemExit(f'unknown game {name!r} ({origin}; known: {", ".join(GAMES)})')
    return name


def selected(name=None, config=None, environ=None):
    """The game a tool works on: `name` (its --game option), else P2_GAME,
    else "game" of the config, else the only game the config sets up (an
    HL2-only checkout needs no --game), else portal2."""
    environ = os.environ if environ is None else environ
    if name: return check(name)
    if environ.get('P2_GAME'): return check(environ['P2_GAME'], 'P2_GAME')
    config = config or {}
    if config.get('game'): return check(config['game'], 'config/local.json')
    if not config.get('game_root'):   # top-level game keys are Portal 2's
        configured = [n for n, section in (config.get('games') or {}).items()
                      if n in GAMES and isinstance(section, dict) and section.get('game_root')]
        if len(configured) == 1: return configured[0]
    return DEFAULT


def add_argument(parser):
    """--game option of the tools (default: selected())."""
    parser.add_argument('--game', choices=sorted(GAMES),
                        help='game to work on (default: P2_GAME, else "game" of config/local.json, else portal2)')


def ios_app(game, target):
    """Path of a profile's iOS app for a target (sim, device), relative to the
    checkout: build/ios/sim/Portal2.app, build/hl2/ios/sim/HalfLife2.app."""
    return Path(game['ios_dir']) / target / game['ios_app']


def profile(config):
    """Profile of the game named by `game` in the config (portal2 when absent);
    localconfig.game_config() returns such a config for any game."""
    name = config.get('game') or DEFAULT
    return dict(GAMES[check(name, 'config/local.json')], name=name)


def ios_userdata(name):
    """The game's saves folder in the app's Documents (runtime/ios/game_ios.c, its
    third column: portal2-userdata, hl2-userdata...), or '' when it has none."""
    import re
    text = (ROOT / 'runtime/ios/game_ios.c').read_text()
    found = re.search(r'\{"' + re.escape(name) + r'", "[\w]+", "([\w-]+)"', text)
    return found.group(1) if found else ''


def gameinfo_dirs(text, mod):
    """Top-level folders a gameinfo.txt's SearchPaths name (Game, GameBin, ...),
    in order: `|gameinfo_path|.` is the mod itself, `|all_source_engine_paths|x`
    and plain `x` are x. Left 4 Dead 2 keeps its shaders in `hl2`, a search
    path of its gameinfo: a device copy without it draws a black screen."""
    import re
    block = re.search(r'SearchPaths\s*\{([^}]*)\}', re.sub(r'//[^\n]*', '', text), re.I)
    found = []
    for line in (block.group(1) if block else '').splitlines():
        parts = line.split()
        if len(parts) < 2:
            continue
        path = parts[-1].strip('"')
        path = re.sub(r'^\|gameinfo_path\|\.?', mod + '/', path)
        path = re.sub(r'^\|all_source_engine_paths\|', '', path)
        top = path.replace('\\', '/').strip('/').split('/')[0]
        if top and top != '.' and top not in found:
            found.append(top)
    return found


def missing_ios_data(game, game_root):
    """Folders the game's gameinfo.txt searches that exist in game_root but that
    its ios_data leaves off the device ([] when the gameinfo isn't there)."""
    info = Path(game_root) / game['mod'] / 'gameinfo.txt'
    if not game_root or not info.is_file():
        return []
    dirs = gameinfo_dirs(info.read_text(errors='replace'), game['mod'])
    return [d for d in dirs if d not in game['ios_data'] and (Path(game_root) / d).is_dir()]

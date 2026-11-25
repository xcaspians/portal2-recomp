#!/usr/bin/env python3
"""Build a recompiled game as an iOS app (arm64 simulator by default).

Same generated C as the macOS build (tools/build_game.py; generation and object
cache reused unchanged), compiled for iOS and linked with the work/ios-deps
dependencies (SDL2, FreeType, gl4es; see its build.sh), the Steam stub and the
minimal fontconfig of the iOS build (runtime/ios). Produces
<ios_dir>/<target>/<ios_app> of the game's profile (tools/games.py):
build/ios/<target>/Portal2.app, build/hl2/ios/<target>/HalfLife2.app; ad hoc
signed for the simulator, or signed with the profile Xcode obtained for a
device (ios/signing). The game (--game, else P2_GAME, else config/local.json)
gives the roots, program, name, icon and Info.plist key P2Game
(runtime/ios/game_ios.c).

    python3 tools/build_ios.py [--game hl2]          # simulator
    tools/run_ios_sim.sh [--game hl2]                # headless run

Metal renderer (native Direct3D 9, docs/d3d9-bridge.md): --d3d9metal links
libd3d9metal.a (mtld3d, make native-ios) into the executable, no dlopen; it is
the default renderer, the player can go back to OpenGL in Settings > <game> > Renderer.
"""
import argparse
import json
import plistlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

from ios_bundle_id import bundle_id

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_game
import games
import language
import localconfig

ROOT = Path(__file__).resolve().parent.parent
FRAMEWORKS = ['UIKit', 'Foundation', 'CoreGraphics', 'QuartzCore', 'OpenGLES', 'Metal', 'AVFoundation',
              'AudioToolbox', 'CoreAudio', 'CoreMotion', 'GameController', 'CoreHaptics', 'CoreBluetooth', 'OpenAL',
              'Security']   # Security: the Steam Web API key in the Keychain (runtime/ios/workshop_ios.m)
# Frameworks de libd3d9metal en plus de ceux de l'app (otool -L de sa dylib iOS).
D3D9METAL_FRAMEWORKS = ['MetalFX', 'CoreVideo', 'CoreFoundation']
TARGETS = {
    'sim': dict(sdk='iphonesimulator', triple='arm64-apple-ios17.0-simulator', platform='ios-simulator',
                plist='iPhoneSimulator'),
    'device': dict(sdk='iphoneos', triple='arm64-apple-ios17.0', platform='ios', plist='iPhoneOS'),
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=sorted(TARGETS), default='sim')
    ap.add_argument('--opt', default='1', choices=['0', '1', '2', '3'])
    ap.add_argument('--fast-memory', action='store_true', help='-DRT_FAST_MEMORY (see docs/fast-path.md)')
    ap.add_argument('--release', action='store_true',
                    help='build to play: --opt 2 --fast-memory, like build_game.py --release')
    ap.add_argument('--debug', action='store_true', default=os.environ.get('P2_IOS_GL_DEBUG') == '1',
                    help='-DRT_IOS_GL_DEBUG: compiles in the GL diagnostics (P2_IOS_GL_TRACE, P2_IOS_SHADER_DUMP, '
                         'P2_IOS_DEBUG_UV, P2_IOS_PROBE_FRAMES, P2_IOS_SKIP_DRAWS, P2_IOS_DRAW_LOG, P2_IOS_SRGB_TRACE; '
                         'docs/ios.md); also P2_IOS_GL_DEBUG=1 in the environment')
    ap.add_argument('--deps', type=Path, help='dependency prefix (default ../work/ios-deps/<target>)')
    ap.add_argument('--d3d9metal', type=Path,
                    help='libd3d9metal.a for this target (default: "d3d9metal_ios" of config/local.json, '
                         'one path per target: {"device": ..., "sim": ...}); without it, OpenGL only')
    games.add_argument(ap)
    ap.add_argument('--layout', type=Path, help='default: <build_dir>/prep/layout.json of the game (build/game, build/hl2)')
    ap.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    ap.add_argument('--no-guests', action='store_true',
                    help="the app's own game only, without the games it hosts (ios_guests of tools/games.py)")
    args = ap.parse_args()
    if args.release:
        args.opt, args.fast_memory = '2', True
    os.chdir(ROOT)
    config = localconfig.for_game(args.game)
    game = games.profile(config)
    if not game['ios']:
        sys.exit(f'build_ios: {game["app_name"]} has no iPhone build yet')
    build_dir, ios_dir = Path(game['build_dir']), Path(game['ios_dir']) / args.target
    args.layout = args.layout or build_dir / 'prep/layout.json'
    target = TARGETS[args.target]
    deps = (args.deps or ROOT.parent / 'work/ios-deps' / args.target).resolve()
    d3d9metal = d3d9metal_library(args.d3d9metal, args.target, config)
    sdk = subprocess.run(['xcrun', '--sdk', target['sdk'], '--show-sdk-path'], check=True,
                         capture_output=True, text=True).stdout.strip()
    started = time.time()
    layout = json.loads(args.layout.read_text())
    manifest, _ = build_game.generate(args.layout, layout, build_game.roots(layout, game['roots']), build_dir / 'gen',
                                      jobs=args.jobs)
    base = ['-target', target['triple'], '-isysroot', sdk]
    memory = ['-DRT_FAST_MEMORY'] if args.fast_memory else []
    debug = ['-DRT_IOS_GL_DEBUG'] if args.debug else []   # runtime/ios/env_ios.h RT_IOS_DEBUG_BUILD (runtime files only)
    runtime_flags = [*base, f'-O{args.opt}', '-g', *build_game.WARNINGS, *memory, *debug, '-Iruntime', '-Iruntime/ios',
                     f'-I{deps}/include', f'-I{deps}/include/SDL2', f'-I{deps}/include/gl4es',
                     f'-I{deps}/include/freetype2', '-DGL_SILENCE_DEPRECATION', '-DGLES_SILENCE_DEPRECATION',
                     '-DOPENAL_DEPRECATED=', '-DAL_SILENCE_DEPRECATION']
    generated_flags = [*base, f'-O{args.opt}', '-gline-tables-only', *build_game.GENERATED_TUNING, *build_game.WARNINGS, *memory, '-Iruntime']
    sources = build_dir / 'gen/src'
    # runtime/d3d9.c : symboles de libd3d9metal.a liés, pas de dlopen ;
    # runtime/ios/metal_ios.m s'en sert pour savoir si Metal est disponible.
    static = ['-DRT_D3D9METAL_STATIC'] if d3d9metal else []
    units = [(str(p), runtime_flags + (static if p.name == 'd3d9.c' else [])) for p in sorted(Path('runtime').glob('*.c'))]
    # guest_glue.c: built with a hosted game's generated code only (guest_object).
    units += [(str(p), runtime_flags) for p in sorted(Path('runtime/ios').glob('*.c')) if p.name != 'guest_glue.c']
    units += [(str(p), runtime_flags + static + ['-fobjc-arc']) for p in sorted(Path('runtime/ios').glob('*.m'))]
    units.append(('src/main.c', runtime_flags + ['-Dmain=p2_main', f'-DRT_GAME_PROGRAM="{game["program"]}"']))
    units.append(('ios/main_ios.c', runtime_flags))
    units += generated_units(config, build_dir, manifest, generated_flags)
    identity = subprocess.run(['clang', '--version'], capture_output=True, text=True, check=True).stdout
    # Games hosted in this app (the Portal app: Mel): their generated code in
    # one object each, the runtime built once with RT_MULTI_GAME (runtime/rt.h).
    guests = [] if args.no_guests else hosted_games(game, args.target)
    if guests:
        units = [(src, flags + ['-DRT_MULTI_GAME'] if flags is runtime_flags or src in ('src/main.c', 'ios/main_ios.c')
                  or src.startswith('runtime/') else flags) for src, flags in units]
        table = ios_dir / 'game_codes.c'
        write_game_codes(table, game['name'], [g['name'] for g, _ in guests])
        units.append((str(table), runtime_flags + ['-DRT_MULTI_GAME']))
    cache = ios_dir / 'obj'
    objects, compiled, cpu = build_game.compile_units(units, cache, args.jobs, identity)
    print(f'{compiled}/{len(units)} fichiers compilés ({cpu:.0f} s CPU)')
    for guest, guest_config in guests:
        objects.append(guest_object(guest, guest_config, args.target, generated_flags, args.jobs, identity, ios_dir))

    app = games.ios_app(game, args.target)
    app.mkdir(parents=True, exist_ok=True)
    response = ios_dir / 'link.rsp'
    build_game.write_link_response(response, objects)
    libs = [str(deps / 'lib' / n) for n in ('libSDL2main.a', 'libSDL2.a', 'libgl4es.a', 'libfreetype.a')]
    frameworks = FRAMEWORKS + (D3D9METAL_FRAMEWORKS if d3d9metal else [])
    link = ['clang', *base, f'@{response}', *libs, *([str(d3d9metal)] if d3d9metal else []), '-liconv',
            *[x for f in frameworks for x in ('-framework', f)],
            # Le SDK 27 impose UIScene, que SDL2 ignore : déclarer un SDK 18
            # garde le cycle de vie classique d'UIKit (voir ios/probe).
            f'-Wl,-platform_version,{target["platform"]},17.0,18.0',
            '-o', str(app / game['ios_executable'])]
    subprocess.run(link, check=True)
    plist = (ROOT / 'ios/Info.plist.in').read_text()
    for key, value in dict(EXECUTABLE=game['ios_executable'], IDENTIFIER=bundle_id(game=game['name']), NAME=game['ios_name'],
                           GAME=game['name'], PLATFORM=target['plist'], FORK_VERSION=fork_version()).items():
        plist = plist.replace(f'@{key}@', value)
    (app / 'Info.plist').write_text(plist)
    # Page du jeu dans l'app Réglages d'iOS (lue par runtime/ios/settings.c).
    shutil.copytree(ROOT / 'ios/Settings.bundle', app / 'Settings.bundle', dirs_exist_ok=True)
    # Défauts par appareil (runtime/ios/device_defaults.c), lus au lancement.
    shutil.copy2(ROOT / 'ios/device_defaults.txt', app / 'device_defaults.txt')
    settings_for_game(app / 'Settings.bundle/Root.plist', game['name'])
    app_icon(app, config, game['icon'])
    (app / 'launch.args').write_text(''.join(f'{word}\n' for word in launch_args(game, config)))
    shutil.rmtree(app / 'covers', ignore_errors=True)
    if guests:   # the game picker's covers and each hosted game's arguments
        cover(app, config, game)
        for guest, guest_config in guests:
            cover(app, guest_config, guest)
            (app / f'launch-{guest["name"]}.args').write_text(''.join(f'{w}\n' for w in launch_args(guest, guest_config)))
    if args.target == 'sim':
        subprocess.run(['codesign', '-s', '-', '-f', str(app)], check=True)
    else:
        subprocess.run([sys.executable, str(ROOT / 'ios/signing/sign.py'), str(app)], check=True)
    size = (app / game['ios_executable']).stat().st_size
    print(f'{app} : exécutable {size / 1e6:.0f} Mo, {time.time() - started:.0f} s au total')
    print(f'rendu Metal : {d3d9metal}' if d3d9metal else 'rendu Metal : absent (OpenGL seulement, voir --d3d9metal)')


def generated_units(config, build_dir, manifest, flags):
    """A game's generated code: its shards, dispatch.c and the Miles PE plugins."""
    sources = build_dir / 'gen/src'
    units = [(str(sources / name), flags) for name in sorted(manifest['shards'])]
    units.append((str(sources / 'dispatch.c'), flags))
    # Greffons PE de Miles traduits (voix MP3...), comme le build macOS.
    plugin_sources, _ = build_game.generate_plugins(config, build_dir / 'gen/pe')
    return units + [(str(p), flags) for p in plugin_sources]


GUEST_EXPORTS = ('rt_load_images', 'rt_export', 'rt_lookup', 'rt_import_name', 'rt_modules', 'rt_module_count')


def hosted_games(game, target):
    """[(profile, config)] of the games this app hosts (ios_guests) that are set up
    on this Mac (game data and a gameprep); a missing one is left out, with a line."""
    out = []
    for name in game.get('ios_guests', ()):
        config = localconfig.for_game(name)
        profile = games.profile(config)
        layout = Path(profile['build_dir']) / 'prep/layout.json'
        if not config.get('game_root') or not layout.is_file():
            print(f'{profile["ios_name"]} : pas préparé sur ce Mac (gameprep --game {name}), app sans lui')
            continue
        out.append((profile, config))
    return out


def guest_object(guest, config, target, generated_flags, jobs, identity, ios_dir):
    """A hosted game's generated code, compiled in its own cache, linked into one
    relocatable object whose only exports are <name>_rt_... (runtime/ios/guest_glue.c):
    its fn_<address> functions never meet the host game's."""
    build_dir = Path(guest['build_dir'])
    layout = json.loads((build_dir / 'prep/layout.json').read_text())
    manifest, _ = build_game.generate(build_dir / 'prep/layout.json', layout, build_game.roots(layout, guest['roots']),
                                      build_dir / 'gen', jobs=jobs)
    units = generated_units(config, build_dir, manifest, generated_flags)
    units.append(('runtime/ios/guest_glue.c', generated_flags + [f'-DRT_GUEST_PREFIX={guest["name"]}']))
    objects, compiled, cpu = build_game.compile_units(units, Path(guest['ios_dir']) / target / 'obj', jobs, identity)
    print(f'{guest["ios_name"]} : {compiled}/{len(units)} fichiers compilés ({cpu:.0f} s CPU)')
    out = ios_dir / f'guest-{guest["name"]}.o'
    exports = ios_dir / f'guest-{guest["name"]}.exports'
    exports.write_text(''.join(f'_{guest["name"]}_{n}\n' for n in GUEST_EXPORTS))
    response = ios_dir / f'guest-{guest["name"]}.rsp'
    build_game.write_link_response(response, objects)
    platform = TARGETS[target]['platform']
    subprocess.run(['xcrun', 'ld', '-r', '-arch', 'arm64', '-platform_version', platform, '17.0', '18.0',
                    '-exported_symbols_list', str(exports), '-o', str(out), f'@{response}'], check=True)
    return out


def write_game_codes(path, host, guests):
    """The app's games for the runtime (rt_game_codes, runtime/rt.h): the host's
    own generated code first, then each hosted game's <name>_rt_... functions."""
    lines = ['/* Generated by tools/build_ios.py: the games of this app (runtime/rt.h, RT_MULTI_GAME). */',
             '#include "rt.h"', '#include "modules.h"', '#include <string.h>',
             '#undef rt_load_images', '#undef rt_export', '#undef rt_lookup', '#undef rt_import_name',
             '#undef rt_modules', '#undef rt_module_count',
             'static const struct GuestModuleS *host_modules(void) { return rt_modules; }',
             'static size_t host_module_count(void) { return rt_module_count; }']
    for g in guests:
        lines += [f'int {g}_rt_load_images(const char *directory);', f'uint32_t {g}_rt_export(const char *name);',
                  f'gfn {g}_rt_lookup(uint32_t target);', f'const char *{g}_rt_import_name(uint32_t target);',
                  f'const struct GuestModuleS *{g}_rt_modules(void);', f'size_t {g}_rt_module_count(void);']
    lines.append('const RtGameCode rt_game_codes[] = {')
    lines.append(f'    {{"{host}", rt_load_images, rt_export, rt_lookup, rt_import_name, host_modules, host_module_count}},')
    for g in guests:
        lines.append(f'    {{"{g}", {g}_rt_load_images, {g}_rt_export, {g}_rt_lookup, {g}_rt_import_name, '
                     f'{g}_rt_modules, {g}_rt_module_count}},')
    lines += ['};', 'const size_t rt_game_code_count = sizeof(rt_game_codes) / sizeof(*rt_game_codes);',
              'const RtGameCode *rt_code = &rt_game_codes[0];',
              'int rt_select_game_code(const char *name) {',
              '    for (size_t i = 0; i < rt_game_code_count; ++i)',
              '        if (!strcmp(rt_game_codes[i].name, name)) { rt_code = &rt_game_codes[i]; return 1; }',
              '    return 0;', '}', '']
    text = '\n'.join(lines)
    if not path.exists() or path.read_text() != text:   # unchanged: same object (cache key)
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text)


def cover(app, config, game):
    """covers/<name>.png for the game picker: 512 px from the game's own icon on this Mac."""
    work = app.parent / 'cover-work'
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    probe = work / 'probe.app'
    probe.mkdir()
    app_icon(probe, config, game['icon'])
    largest = probe / 'AppIcon60x60@3x.png'
    source = max(work.glob('icon/game.iconset/icon_*.png'), key=lambda p: p.stat().st_size, default=None) \
        or (work / 'icon/game.png' if (work / 'icon/game.png').is_file() else None) or (largest if largest.is_file() else None)
    if source:
        (app / 'covers').mkdir(exist_ok=True)
        subprocess.run(['sips', '-z', '512', '512', str(source), '--out', str(app / 'covers' / f'{game["name"]}.png')],
                       check=True, capture_output=True)
    shutil.rmtree(work, ignore_errors=True)


# Version of Jacksen's fork, shown at the bottom of the in-game settings menu
# (Info.plist key P2ForkVersion, settings_menu_ios.m).
FORK_VERSION = '0.4.0-fork.2'


def fork_version(root=ROOT):
    """FORK_VERSION and the commit it was built from: "0.4.0-fork.2 (69264e7)",
    "+" after the hash when the tree had uncommitted changes."""
    def git(*args):
        result = subprocess.run(['git', '-C', str(root), *args], capture_output=True, text=True)
        return result.stdout.strip() if result.returncode == 0 else ''
    commit = git('rev-parse', '--short=7', 'HEAD')
    if not commit: return FORK_VERSION
    dirty = git('status', '--porcelain', '--untracked-files=no')
    return f'{FORK_VERSION} ({commit}{"+" if dirty else ""})'


def launch_args(game, config):
    """Arguments of a launch from the home screen (launch.args; the data
    folders are added by ios/main_ios.c). The language comes from
    tools/language.py at build time, for this game's config (its folder and
    appmanifest): to change it, set "language" in config/local.json and
    rebuild."""
    words = games.forced_args(game, ['-game', game['mod'], '-novid', '-fullscreen'])   # L4D2: offline, always
    return ['--', *words, *language.language_args(config, words)]


def d3d9metal_library(given, target, config):
    """libd3d9metal.a to link for this target (argument, else config/local.json), or None."""
    if given is None:
        entry = config.get('d3d9metal_ios')
        given = entry.get(target) if isinstance(entry, dict) else None
        if not given:
            return None
    path = Path(given).expanduser().resolve()
    if path.suffix != '.a' or not path.is_file():
        sys.exit(f'--d3d9metal : {path} n\'est pas une bibliothèque statique (libd3d9metal.a de make native-ios)')
    return path


def settings_for_game(root_plist, game):
    """Half-Life 2 has no portals: the Portals group of the settings page (back
    taps, portal buttons, vibration when firing) is left out, back taps stay
    off (runtime/ios/game_ios.c), and gyro aiming is shown on, its default there."""
    if game in ('portal2', 'portal1', 'mel'):   # Mel plays like Portal 2: portals and back taps
        return
    with open(root_plist, 'rb') as f:
        root = plistlib.load(f)
    kept, skipping = [], False
    for item in root['PreferenceSpecifiers']:
        if item.get('Type') == 'PSGroupSpecifier':
            skipping = item.get('Title') == 'Portals'
        if item.get('Key') == 'P2Gyro':
            item['DefaultValue'] = True   # gyro aiming on by default (touch_controls.m)
        if not skipping:
            kept.append(item)
    root['PreferenceSpecifiers'] = kept
    with open(root_plist, 'wb') as f:
        plistlib.dump(root, f, fmt=plistlib.FMT_BINARY)


def app_icon(app, config, relative):
    """App icon taken from the game's macOS icon (`icon` of the profile, e.g.
    portal2/resource/game.icns, under the game's game_root in `config`): a
    Valve image, read from the player's copy instead of being versioned.
    Old-style files (CFBundleIconFiles of ios/Info.plist.in), no asset
    catalog. Without the file: iOS default icon."""
    icns = Path(config.get('game_root', '')) / relative
    if not icns.is_file():
        print(f'icône : {icns} absent, icône par défaut')
        return
    work = app.parent / 'icon'
    shutil.rmtree(work, ignore_errors=True)
    if icns.suffix.lower() == '.ico':   # Portal Stories: Mel ships portal2.ico (largest image read by sips)
        work.mkdir(parents=True)
        largest = work / 'game.png'
        subprocess.run(['sips', '-s', 'format', 'png', str(icns), '--out', str(largest)], check=True, capture_output=True)
    else:
        subprocess.run(['iconutil', '-c', 'iconset', '-o', str(work / 'game.iconset'), str(icns)], check=True)
        largest = max((work / 'game.iconset').glob('icon_*.png'), key=lambda p: p.stat().st_size)
    for name, size in (('AppIcon60x60@2x.png', 120), ('AppIcon60x60@3x.png', 180), ('AppIcon76x76@2x~ipad.png', 152),
                       ('AppIcon83.5x83.5@2x~ipad.png', 167)):
        subprocess.run(['sips', '-z', str(size), str(size), str(largest), '--out', str(app / name)],
                       check=True, capture_output=True)


if __name__ == '__main__':
    main()

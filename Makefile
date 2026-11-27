RUNTIME := $(sort $(wildcard runtime/*.c))
CHECKS := $(sort $(wildcard tests/checks_*.c))
PYTHON ?= python3
CC = clang
CFLAGS ?= -O2 -g -Wall -Wextra -Werror
# config/local.json, read once (tools/localconfig.py --make: KEY=value words,
# spaces encoded as %20): host_prefix (P2_HOST_PREFIX, the config's prebuilt
# bundle, else Homebrew) and Portal 2's linux_root.
LOCAL := $(shell $(PYTHON) tools/localconfig.py --make host_prefix linux_root --game portal2 2>/dev/null)
local_value = $(subst %20, ,$(patsubst $(1)=%,%,$(filter $(1)=%,$(LOCAL))))
# Bibliothèques hôtes ARM64 des ponts natifs ; partagées avec tools/build_game.py
# (localconfig.host_flags).
HOST_PREFIX := $(or $(P2_HOST_PREFIX),$(call local_value,host_prefix),/opt/homebrew)
HOST_CFLAGS = -I"$(HOST_PREFIX)/include" -I"$(HOST_PREFIX)/include/freetype2" -DGL_SILENCE_DEPRECATION -DOPENAL_DEPRECATED= -DAL_SILENCE_DEPRECATION
HOST_LIBS = -L"$(HOST_PREFIX)/lib" -Wl,-rpath,"$(HOST_PREFIX)/lib" -lSDL2 -lfreetype -lfontconfig -liconv -framework OpenGL -framework OpenAL -lobjc
CFLAGS += $(HOST_CFLAGS)
LIFT_FLAGS ?= --allow-unimplemented-imports
# Linux binaries of the game (libtier0.so, libvstdlib.so): linux_root of
# config/local.json when it exists, else the historical private path.
# INPUT_DIR_DEP is the same path with spaces escaped for prerequisites.
LOCAL_LINUX_ROOT := $(call local_value,linux_root)
INPUT_DIR ?= $(if $(LOCAL_LINUX_ROOT),$(LOCAL_LINUX_ROOT)/bin/linux32,../work/portal2-linux/bin)
space := $(subst ,, )
INPUT_DIR_DEP := $(subst $(space),\ ,$(INPUT_DIR))
PREP = build/prep
EXPORTS = Plat_IsInBenchmarkMode Plat_SetBenchmarkMode AreAllAssertsDisabled SetAllAssertsDisabled UnitTestCount GetUnitTest EncodeBinaryToString DecodeBinaryFromString Plat_GetMemPageSize ZeroMemory Plat_Alloc Plat_Realloc Plat_Free Plat_DefaultAllocErrorFn UnitTestInstallTestCase LoggingSystem_GetChannelCount LoggingSystem_FindChannel libtier0.so:code:0x157a0 libtier0.so:code:0x15a50 libtier0.so:code:0x15620 libtier0.so:code:0x13dd0 libtier0.so:code:0x13e00 libtier0.so:code:0x13e30 libtier0.so:code:0x13e60 libtier0.so:init:dt libtier0.so:init:0 libtier0.so:init:1 libtier0.so:init:2 libtier0.so:init:3 libtier0.so:init:4 libtier0.so:init:5 libtier0.so:init:6 libtier0.so:init:7 libtier0.so:init:8 libtier0.so:init:9 libtier0.so:init:10 libtier0.so:init:11 libtier0.so:init:12 libtier0.so:init:13 GetCPUInformation

# Objects: one per C file under build/obj (build/obj-ubsan for the UBSan
# variant), with clang's dependency files (-MMD), so `make -j8 check` compiles
# in parallel and only what changed. Same flags as the former one-shot links.
OBJ := build/obj
OBJ_UBSAN := build/obj-ubsan
UBSAN_FLAGS := -O1 -fsanitize=undefined -fno-sanitize-recover=all
ABI_SOURCES := $(RUNTIME) build/abi-fixture.c build/pe-fixture.c tests/abi_smoke.c $(CHECKS)
TIER0_SOURCES := $(RUNTIME) build/tier0.c tests/tier0_smoke.c
ABI_OBJECTS := $(patsubst %.c,$(OBJ)/%.o,$(ABI_SOURCES))
TIER0_OBJECTS := $(patsubst %.c,$(OBJ)/%.o,$(TIER0_SOURCES))
ABI_OBJECTS_UBSAN := $(patsubst %.c,$(OBJ_UBSAN)/%.o,$(ABI_SOURCES))
TIER0_OBJECTS_UBSAN := $(patsubst %.c,$(OBJ_UBSAN)/%.o,$(TIER0_SOURCES))

.DEFAULT_GOAL := check
.PHONY: all test check clean
all: build/tier0-smoke

# build/obj/flags holds the compiler flags of the current build: a change of
# HOST_PREFIX or CFLAGS (setup.sh switching to the prebuilt bundle) recompiles
# every object, which -MMD alone would not notice.
FLAGS_STAMP := $(OBJ)/flags
$(shell mkdir -p $(OBJ); [ "$$(cat $(FLAGS_STAMP) 2>/dev/null)" = "$(CC) $(CFLAGS) $(UBSAN_FLAGS)" ] || echo "$(CC) $(CFLAGS) $(UBSAN_FLAGS)" > $(FLAGS_STAMP))
$(OBJ)/%.o: %.c $(FLAGS_STAMP)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -Iruntime -Itests -MMD -MP -c $< -o $@
$(OBJ_UBSAN)/%.o: %.c $(FLAGS_STAMP)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) -Iruntime -Itests -MMD -MP -c $< -o $@
-include $(ABI_OBJECTS:.o=.d) $(TIER0_OBJECTS:.o=.d) $(ABI_OBJECTS_UBSAN:.o=.d) $(TIER0_OBJECTS_UBSAN:.o=.d)

# $(PREP)/input-dir records the INPUT_DIR in use, so a changed depot regenerates the images.
.PHONY: .FORCE
.FORCE:
$(PREP)/input-dir: .FORCE
	@mkdir -p $(PREP); [ "$$(cat $@ 2>/dev/null)" = "$(INPUT_DIR)" ] || echo "$(INPUT_DIR)" > $@
$(PREP)/layout.json: tools/elfprep.py $(INPUT_DIR_DEP)/libtier0.so $(INPUT_DIR_DEP)/libvstdlib.so $(PREP)/input-dir
	$(PYTHON) tools/elfprep.py --out $(PREP) "$(INPUT_DIR)/libtier0.so@0x10000000" "$(INPUT_DIR)/libvstdlib.so@0x11000000"

build/tier0.c: Makefile tools/lift.py tools/float_lift.py tools/registry.py $(PREP)/layout.json
	$(PYTHON) tools/lift.py $(LIFT_FLAGS) --layout $(PREP)/layout.json --out $@ $(EXPORTS)

build/tier0-smoke: $(TIER0_OBJECTS)
	$(CC) $(CFLAGS) $^ $(HOST_LIBS) -o $@

build/abi-fixture.c: tests/generate_abi_fixture.py tools/lift.py tools/float_lift.py tools/registry.py tests/lifter_programs.s
	@mkdir -p build
	$(PYTHON) tests/generate_abi_fixture.py $@

# Greffon PE de test (DLL liée par tests/pe_fixture.py, traduite par lift.generate_pe).
build/pe-fixture.c: tests/generate_pe_fixture.py tests/pe_fixture.py tests/pe_plugin.s tools/lift.py tools/float_lift.py tools/peprep.py
	@mkdir -p build
	$(PYTHON) tests/generate_pe_fixture.py $@

# Native plugins loaded by tests/checks_native_plugin.c (dlopen at run time, not linked).
PLUGIN_FIXTURES := build/native-plugin-ok.dylib build/native-plugin-refuse.dylib
build/native-plugin-ok.dylib: tests/native_plugin_fixture.c runtime/native_plugin.h
	@mkdir -p build
	$(CC) $(CFLAGS) -Iruntime -dynamiclib $< -o $@
build/native-plugin-refuse.dylib: tests/native_plugin_fixture.c runtime/native_plugin.h
	@mkdir -p build
	$(CC) $(CFLAGS) -Iruntime -DREFUSE -dynamiclib $< -o $@

build/abi-smoke: $(ABI_OBJECTS) | $(PLUGIN_FIXTURES)
	$(CC) $(CFLAGS) $^ $(HOST_LIBS) -o $@

# Full run: needs the game's Linux binaries (INPUT_DIR) for tier0-smoke.
test: build/tier0-smoke build/abi-smoke
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py' -v
	./build/tier0-smoke $(PREP)
	./build/abi-smoke

# What CI runs (make -j check): Python tests and the runtime against generated fixtures, no game files needed.
check: build/abi-smoke build/x64-check build/x64-dylib-check build/pe-check
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py'
	./build/abi-smoke
	./build/x64-check
	./build/x64-dylib-check
	./build/pe-check

clean:
	rm -rf build

# The Natif Installer as a DMG (docs/usage.md#releases): signed and notarized
# when this Mac has a Developer ID certificate and the p2-notary profile.
installer-dmg:
	tools/build_installer.sh --dmg

.PHONY: progress sanitize game installer-dmg
# Whole game: every known function, C split and compiled in parallel with an
# object cache (build/game/obj; build/hl2/obj for GAME=hl2). GAME_OPT=0 for a
# diagnostic binary. GAME=portal2|hl2|portal1 (default: P2_GAME, then "game" of
# config/local.json, then portal2; tools/games.py).
GAME_OPT ?= 1
GAME ?=
game:
	$(PYTHON) tools/build_game.py --opt $(GAME_OPT) $(if $(GAME),--game $(GAME))

progress: $(PREP)/layout.json
	$(PYTHON) tools/progress.py --layout $(PREP)/layout.json --out build/progress.json

build/tier0-smoke-ubsan: $(TIER0_OBJECTS_UBSAN)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) $^ $(HOST_LIBS) -o $@

build/abi-smoke-ubsan: $(ABI_OBJECTS_UBSAN) | $(PLUGIN_FIXTURES)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) $^ $(HOST_LIBS) -o $@

sanitize: build/tier0-smoke-ubsan build/abi-smoke-ubsan
	./build/tier0-smoke-ubsan $(PREP)
	./build/abi-smoke-ubsan

# libd3d9metal seule, liée statiquement comme dans l'app iOS (sans le pont) :
# make d3d9metal-check D3D9METAL_A=…/mtld3d/native/target/release/libd3d9metal.a
.PHONY: d3d9metal-check
D3D9METAL_LIBS = -framework Metal -framework MetalFX -framework QuartzCore -framework CoreGraphics -framework CoreVideo \
	-framework Foundation -framework CoreFoundation -lobjc -liconv
build/d3d9metal-check: tests/d3d9metal_native_check.c runtime/d3d9/d3d9metal.h $(D3D9METAL_A)
	@test -n "$(D3D9METAL_A)" || { echo "D3D9METAL_A=/path/to/libd3d9metal.a (mtld3d: make native)"; exit 2; }
	$(CC) -O1 -g -Wall -Wextra -Werror tests/d3d9metal_native_check.c $(D3D9METAL_A) $(D3D9METAL_LIBS) -o $@
d3d9metal-check: build/d3d9metal-check
	./build/d3d9metal-check

# x86-64 (P2CE, docs/fork/P2CE_PLAN.md): its own runtime (runtime/x64), never
# linked into the i386 binaries. build/x64-check lifts tests/x64_programs.s
# (linked by tests/x64_link.py, no ELF linker needed) and runs it; part of
# `make check`.
X64_RUNTIME := $(sort $(wildcard runtime/x64/*.c))
X64_CFLAGS ?= -O1 -g -Wall -Wextra -Werror
X64_LIBS = -liconv
build/x64/libx64fixture.so: tests/x64_programs.s tests/x64_link.py
	@mkdir -p build/x64
	$(CC) -target x86_64-linux-gnu -c tests/x64_programs.s -o build/x64/x64_programs.o
	$(PYTHON) tests/x64_link.py build/x64/x64_programs.o $@
build/x64/fixture.c: build/x64/libx64fixture.so tools/elfprep64.py tools/lift64.py tools/simd64.py tools/x87_64.py
	$(PYTHON) tools/elfprep64.py --out build/x64 build/x64/libx64fixture.so
	$(PYTHON) tools/lift64.py --layout build/x64/libx64fixture.so.layout.json --all --symbol x64_fixture --out $@
build/x64/libx64second.so: tests/x64_programs2.s tests/x64_link.py
	@mkdir -p build/x64
	$(CC) -target x86_64-linux-gnu -c tests/x64_programs2.s -o build/x64/x64_programs2.o
	$(PYTHON) tests/x64_link.py --needed libx64fixture.so build/x64/x64_programs2.o $@
build/x64/second.c: build/x64/libx64second.so tools/elfprep64.py tools/lift64.py tools/simd64.py tools/x87_64.py
	$(PYTHON) tools/elfprep64.py --out build/x64 build/x64/libx64second.so
	$(PYTHON) tools/lift64.py --layout build/x64/libx64second.so.layout.json --all --symbol x64_second --out $@
build/x64-check: $(X64_RUNTIME) runtime/x64/rt64.h build/x64/fixture.c build/x64/second.c tests/x64_checks.c
	$(CC) $(X64_CFLAGS) -Iruntime $(X64_RUNTIME) build/x64/fixture.c build/x64/second.c tests/x64_checks.c $(X64_LIBS) -o $@
# PE32+ (Windows x86-64) front end: tests/pe_programs.s assembled for
# x86_64-windows-msvc, linked into a DLL by tests/pe_link.py (no PE linker on
# the Mac), prepared by tools/peprep64.py, lifted and run by tests/pe_checks.c.
PE_FIXTURE_EXPORTS = pe_add5 pe_teb_self pe_tls_bump pe_switch pe_switch2 pe_switch3 pe_bnd pe_setjmp pe_import_twice pe_import_sum5 \
	pe_import_ordinal pe_guard_call pe_hidden_call
build/pe/pefix.dll: tests/pe_programs.s tests/pe_link.py
	@mkdir -p build/pe
	$(CC) -target x86_64-windows-msvc -c tests/pe_programs.s -o build/pe/pe_programs.obj
	$(PYTHON) tests/pe_link.py --name pefix.dll --import twice=pehost.dll --import sum5=pehost.dll \
		--import seven=pehost.dll#7 --import _setjmp=pehost.dll --import longjmp=pehost.dll $(addprefix --export ,$(PE_FIXTURE_EXPORTS)) build/pe/pe_programs.obj $@
build/pe/fixture.c: build/pe/pefix.dll tools/peprep64.py tools/lift64.py tools/simd64.py tools/x87_64.py
	$(PYTHON) tools/peprep64.py --out build/pe build/pe/pefix.dll
	$(PYTHON) tools/lift64.py --layout build/pe/pefix.dll.layout.json --all --symbol pe_fixture --out $@
build/pe-check: $(X64_RUNTIME) runtime/x64/rt64.h runtime/x64/win64.h build/pe/fixture.c tests/pe_checks.c
	$(CC) $(X64_CFLAGS) -Iruntime $(X64_RUNTIME) build/pe/fixture.c tests/pe_checks.c $(X64_LIBS) -o $@
# The same checks with each lifted module in its own dylib and the runtime in
# librt64.dylib (tools/link64.py), as make p2ce-launch links P2CE since round 28.
build/x64-dylib-check: $(X64_RUNTIME) runtime/x64/rt64.h build/x64/fixture.c build/x64/second.c tests/x64_checks.c tools/link64.py
	@mkdir -p build/x64-dylib
	$(CC) -O1 -g0 -w -Iruntime -c build/x64/fixture.c -o build/x64-dylib/fixture.o
	$(CC) -O1 -g0 -w -Iruntime -c build/x64/second.c -o build/x64-dylib/second.o
	rm -f build/x64-dylib/libm_x64_fixture.a build/x64-dylib/libm_x64_second.a
	ar rcs build/x64-dylib/libm_x64_fixture.a build/x64-dylib/fixture.o
	ar rcs build/x64-dylib/libm_x64_second.a build/x64-dylib/second.o
	$(PYTHON) tools/link64.py --out build/x64-dylib --exe $@ --main tests/x64_checks.c \
		build/x64-dylib/libm_x64_fixture.a build/x64-dylib/libm_x64_second.a

# Live milestone (needs P2CE's depot 440005, never part of `make check`):
# lifts P2CE's own libtier0.so into build/p2ce-tier0 and calls some of its
# exports; the coverage of the whole module goes to build/p2ce-tier0/coverage.json.
# P2_P2CE_TESTS=1 runs it from the Python tests (tests/test_p2ce_live.py).
P2CE_ROOT ?= $(HOME)/Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS/steamapps/content/app_440000/depot_440005
P2CE_TIER0_ROOTS = Plat_IsInBenchmarkMode Plat_SetBenchmarkMode V_tier0_strncpy V_tier0_stricmp \
	ThreadInterlockedCompareExchange64 ThreadInterlockedExchangePointer ZeroMemory Plat_FloatTime \
	LoggingSystem_FindChannel
.PHONY: p2ce-tier0-check
p2ce-tier0-check: $(X64_RUNTIME) runtime/x64/rt64.h tests/x64_tier0_live.c tools/elfprep64.py tools/lift64.py
	@if [ ! -f "$(P2CE_ROOT)/bin/linux64/libtier0.so" ]; then \
		echo "p2ce-tier0-check: skipped: no bin/linux64/libtier0.so under P2CE_ROOT=$(P2CE_ROOT) (download_depot 440000 440005)"; exit 0; fi; \
	set -e; mkdir -p build/p2ce-tier0; \
	$(PYTHON) tools/elfprep64.py --out build/p2ce-tier0 "$(P2CE_ROOT)/bin/linux64/libtier0.so"; \
	$(PYTHON) tools/lift64.py --layout build/p2ce-tier0/libtier0.so.layout.json --all --report build/p2ce-tier0/coverage.json; \
	$(PYTHON) tools/lift64.py --layout build/p2ce-tier0/libtier0.so.layout.json --symbol p2ce_tier0 \
		--out build/p2ce-tier0/tier0.c $(P2CE_TIER0_ROOTS); \
	$(CC) $(X64_CFLAGS) -Iruntime $(X64_RUNTIME) build/p2ce-tier0/tier0.c tests/x64_tier0_live.c $(X64_LIBS) -o build/p2ce-tier0-check; \
	./build/p2ce-tier0-check

# P2CE plan, round B: `make p2ce-coverage` sweeps every x86-64 module of the
# depot (tools/coverage64.py, about 15 minutes, build/p2ce-coverage/summary.json);
# `make p2ce-build` lifts and compiles whole modules into static libraries
# (tools/build64.py, two compiles at a time, waits while swap is above 6 GB).
P2CE_BUILD_MODULES ?= libtier0.so libvstdlib.so
.PHONY: p2ce-coverage p2ce-build
p2ce-coverage:
	@if [ ! -d "$(P2CE_ROOT)/bin/linux64" ]; then echo "p2ce-coverage: skipped: no P2CE depot at P2CE_ROOT=$(P2CE_ROOT)"; exit 0; fi; \
	$(PYTHON) tools/coverage64.py --out build/p2ce-coverage "$(P2CE_ROOT)"
p2ce-build:
	@if [ ! -d "$(P2CE_ROOT)/bin/linux64" ]; then echo "p2ce-build: skipped: no P2CE depot at P2CE_ROOT=$(P2CE_ROOT)"; exit 0; fi; \
	$(PYTHON) tools/build64.py --out build/p2ce/lib $(foreach m,$(P2CE_BUILD_MODULES),"$(P2CE_ROOT)/bin/linux64/$(m)")

# Round C: libtier0 + libvstdlib + filesystem_stdio (+ libmimalloc.so, which
# carries P2CE's C++ runtime) loaded together, their
# constructors run, CommandLine()->CheckParm() (tests/x64_modules_live.c).
P2CE_MODULES_C = libmimalloc.so libtier0.so libvstdlib.so filesystem_stdio.so
P2CE_LIBS = build/p2ce/lib/m_mimalloc/libm_mimalloc.a build/p2ce/lib/m_tier0/libm_tier0.a build/p2ce/lib/m_vstdlib/libm_vstdlib.a \
	build/p2ce/lib/m_filesystem_stdio/libm_filesystem_stdio.a
.PHONY: p2ce-modules-check
p2ce-modules-check: $(X64_RUNTIME) runtime/x64/rt64.h tests/x64_modules_live.c tests/x64_except.cpp
	@if [ ! -d "$(P2CE_ROOT)/bin/linux64" ]; then echo "p2ce-modules-check: skipped: no P2CE depot at P2CE_ROOT=$(P2CE_ROOT)"; exit 0; fi; \
	set -e; mkdir -p build/x64; \
	$(CC) -target x86_64-linux-gnu -fPIC -O1 -fexceptions -c tests/x64_except.cpp -o build/x64/x64_except.o; \
	$(PYTHON) tests/x64_link.py --needed libmimalloc.so build/x64/x64_except.o build/x64/libx64except.so; \
	$(PYTHON) tools/build64.py --out build/p2ce/lib $(foreach m,$(P2CE_MODULES_C),"$(P2CE_ROOT)/bin/linux64/$(m)") \
		build/x64/libx64except.so; \
	$(CC) $(X64_CFLAGS) -Iruntime $(X64_RUNTIME) tests/x64_modules_live.c $(P2CE_LIBS) \
		build/p2ce/lib/m_x64except/libm_x64except.a $(X64_LIBS) -o build/p2ce-modules-check; \
	./build/p2ce-modules-check

# Round D: P2CE's own program (bin/linux64/p2ce -> launcher.so -> engine.so)
# lifted with the modules it loads, run from a mirror of the game folder
# (tools/run64.py; the depot is never written). P2CE_ARGS are the game's.
# libsteam_api.so is not lifted: its functions go to Valve's macOS
# libsteam_api.dylib (runtime/x64/steam64.c, round 23), with Steam running.
# Round 27: the engine's systems, with the empty renderer (-shaderapi shaderapiempty)
# until the renderer is decided (docs/fork/P2CE_RENDERER_DECISION.md).
# Round 28: video_services (the launcher stops without it). The launcher also
# loads richpresence, toolframework, vguimatsurface, workshop, panorama and
# panoramauiclient: they lift, but one executable holding them all no longer
# links (arm64 branches reach +/-128 MB). Since then each module is its own
# dylib (tools/link64.py: build/p2ce/dylib), with the runtime in librt64.dylib.
P2CE_SYSTEMS ?= bin/linux64/shaderapiempty.so bin/linux64/inputsystem.so bin/linux64/materialsystem.so \
	bin/linux64/datacache.so bin/linux64/studiorender.so bin/linux64/vphysics.so bin/linux64/vphysics_jolt.so \
	bin/linux64/engineaudio.so bin/linux64/soundemittersystem.so bin/linux64/scriptsystem.so bin/linux64/vscript.so \
	bin/linux64/localize.so bin/linux64/scenefilecache.so bin/linux64/vgui2.so \
	bin/linux64/video_services.so bin/linux64/richpresence.so bin/linux64/toolframework.so bin/linux64/vguimatsurface.so \
	bin/linux64/workshop.so bin/linux64/panorama.so bin/linux64/panoramauiclient.so \
	bin/linux64/libfreetype.so.6 bin/linux64/libfontconfig.so.1 bin/linux64/devui.so \
	bin/linux64/libvideo.so bin/linux64/libavcodec.so.57 bin/linux64/libavformat.so.57 bin/linux64/libavresample.so.3 \
	bin/linux64/libavutil.so.55 bin/linux64/libswscale.so.4 bin/linux64/libbz2.so.1.0
# libvideo.so links the system's libstdc++ (the other modules carry theirs in
# libmimalloc.so): Debian's x86-64 copy, downloaded by tools/fetch_debian_amd64.sh.
P2CE_SYSTEM_LIBS ?= libstdc++.so.6 libgcc_s.so.1
# Round 28 (plan round E): SDL is the Mac's own SDL2, not P2CE's Linux copy
# lifted: its imports and dlopen/dlsym go to the native library
# (runtime/x64/native_libs64.h), so windows, the keyboard and the mouse are
# Cocoa's. The host-library bundle's, else Homebrew's.
P2CE_SDL2 ?= $(firstword $(wildcard deps/p2-host-deps-1-macos-arm64/lib/libSDL2-2.0.0.dylib /opt/homebrew/lib/libSDL2-2.0.0.dylib))
P2CE_NATIVE_LIBS ?= libSDL2-2.0.so.0=$(CURDIR)/$(P2CE_SDL2)
P2CE_LAUNCH_MODULES ?= bin/linux64/p2ce bin/linux64/libmimalloc.so bin/linux64/libtier0.so \
	bin/linux64/libvstdlib.so bin/linux64/launcher.so \
	bin/linux64/filesystem_stdio.so bin/linux64/engine.so $(P2CE_SYSTEMS)
P2CE_STEAM_API ?= $(CURDIR)/deps/steamworks-2024.8.0/steam_api.bundle/Contents/MacOS/libsteam_api.dylib
P2CE_APPID ?= 440000
P2CE_ARGS ?= -game p2ce -textmode -novid -nosound -shaderapi shaderapiempty -nominidumps -nobreakpad -nocrashdialog
P2CE_CONTENT ?= $(subst depot_440005,depot_440002,$(P2CE_ROOT))
P2CE_TIMEOUT ?= 120
# -O0 by default: engine.so is 170 C files; -O1 takes about 2 GB and minutes per file.
P2CE_OPT ?= 0
# build64 waits while the Mac's used swap is above this (macOS keeps swap after memory frees up).
P2CE_MAX_SWAP_GB ?= 6
# Round 28: which Vulkan features DXVK needs and MoltenVK lacks (MoltenVK built in
# deps/moltenvk-src, DXVK's headers in deps/dxvk-src; never part of make check).
MOLTENVK_LIB ?= deps/moltenvk-src/Package/Release/MoltenVK/dynamic/dylib/macOS
.PHONY: p2ce-moltenvk-features
p2ce-moltenvk-features: tools/moltenvk_features.c
	@mkdir -p build/p2ce
	$(CC) -Ideps/dxvk-src/include/vulkan/include tools/moltenvk_features.c $(MOLTENVK_LIB)/libMoltenVK.dylib \
		"-Wl,-rpath,$(CURDIR)/$(MOLTENVK_LIB)" -o build/p2ce/moltenvk-features
	./build/p2ce/moltenvk-features 2>/dev/null
# Round 28 (plan round F): DXVK 1.10 native on MoltenVK, built by
# tools/build_dxvk_macos.sh, clears an SDL window and reads the colour back.
# Never part of make check (fetches and builds DXVK, MoltenVK, glslang).
P2CE_DXVK ?= deps/dxvk-macos
.PHONY: p2ce-dxvk-clear
p2ce-dxvk-clear: tools/dxvk_clear_test.cpp tools/build_dxvk_macos.sh
	tools/build_dxvk_macos.sh >/dev/null
	@mkdir -p build/p2ce
	clang++ -std=c++17 -w -DDXVK_NATIVE -Ideps/dxvk-macos-src/include/native/windows \
		-Ideps/dxvk-macos-src/include/native/directx -Ideps/dxvk-macos-src/include/native -Ideps/dxvk-macos-src/include \
		-I$(dir $(P2CE_SDL2))../include/SDL2 tools/dxvk_clear_test.cpp $(P2CE_DXVK)/libdxvk_d3d11.dylib \
		$(P2CE_DXVK)/libdxvk_dxgi.dylib $(P2CE_SDL2) "-Wl,-rpath,$(CURDIR)/$(P2CE_DXVK)" \
		"-Wl,-rpath,$(CURDIR)/$(dir $(P2CE_SDL2))" -o build/p2ce/dxvk-clear
	SDL_VULKAN_LIBRARY="$(CURDIR)/$(P2CE_DXVK)/libMoltenVK.dylib" ./build/p2ce/dxvk-clear 1
# The renderer path (plan rounds E and F): P2CE's shaderapidx11.so lifted,
# DXVK (tools/build_dxvk_macos.sh) and the Mac's SDL2 as native libraries,
# SDL's Vulkan from MoltenVK, a window instead of -textmode. Round 28 stops
# at Panorama's first V8 call (plan round G), before the window opens.
P2CE_DX11_SYSTEMS = $(P2CE_SYSTEMS) bin/linux64/shaderapidx11.so bin/linux64/stdshader_dx11.so
P2CE_DX11_NATIVE = $(P2CE_NATIVE_LIBS);libdxvk_d3d11.so.0=$(CURDIR)/$(P2CE_DXVK)/libdxvk_d3d11.dylib;libdxvk_dxgi.so.0=$(CURDIR)/$(P2CE_DXVK)/libdxvk_dxgi.dylib
P2CE_DX11_ARGS ?= -game p2ce -novid -nosound -shaderapi shaderapidx11 -windowed -w 1280 -h 720 -nominidumps -nobreakpad -nocrashdialog
.PHONY: p2ce-launch-dx11
p2ce-launch-dx11:
	tools/build_dxvk_macos.sh >/dev/null
	SDL_VULKAN_LIBRARY="$(CURDIR)/$(P2CE_DXVK)/libMoltenVK.dylib" $(MAKE) p2ce-launch \
		P2CE_SYSTEMS="$(P2CE_DX11_SYSTEMS)" P2CE_NATIVE_LIBS="$(P2CE_DX11_NATIVE)" P2CE_ARGS="$(P2CE_DX11_ARGS)"
.PHONY: p2ce-launch
p2ce-launch: $(X64_RUNTIME) runtime/x64/rt64.h src/main64.c tools/run64.py
	@if [ ! -d "$(P2CE_ROOT)/bin/linux64" ]; then echo "p2ce-launch: skipped: no P2CE depot at P2CE_ROOT=$(P2CE_ROOT)"; exit 0; fi; \
	set -e; SYSLIB=$$(tools/fetch_debian_amd64.sh); \
	$(PYTHON) tools/build64.py --out build/p2ce/lib --opt $(P2CE_OPT) --max-swap-gb $(P2CE_MAX_SWAP_GB) --registry build/p2ce/registry.c \
		$(foreach m,$(P2CE_LAUNCH_MODULES),"$(P2CE_ROOT)/$(m)") \
		$(foreach m,$(P2CE_SYSTEM_LIBS),"$$SYSLIB/$(m)"); \
	$(PYTHON) tools/link64.py --out build/p2ce/dylib --exe build/p2ce/p2ce-run --main src/main64.c \
		--registry build/p2ce/registry.c \
		$$(sed -n 's/^extern const RT64Module \(m_[a-z0-9_]*\);/build\/p2ce\/lib\/\1\/lib\1.a/p' build/p2ce/registry.c); \
	P2_STEAM_API_DYLIB="$(P2CE_STEAM_API)" P2_STEAM_APPID=$(P2CE_APPID) RT64_NATIVE_LIBS="$(P2CE_NATIVE_LIBS)" \
	$(PYTHON) tools/run64.py --depot "$(P2CE_ROOT)" --depot "$(P2CE_CONTENT)" --mirror build/p2ce/game --timeout $(P2CE_TIMEOUT) \
		build/p2ce/p2ce-run $(P2CE_ARGS)

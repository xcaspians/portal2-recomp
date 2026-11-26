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
check: build/abi-smoke build/x64-check
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py'
	./build/abi-smoke
	./build/x64-check

clean:
	rm -rf build

.PHONY: progress sanitize game
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
# links (arm64 branches reach +/-128 MB), so they wait for a split into
# libraries (docs/fork/P2CE_PLAN.md).
P2CE_SYSTEMS ?= bin/linux64/shaderapiempty.so bin/linux64/inputsystem.so bin/linux64/materialsystem.so \
	bin/linux64/datacache.so bin/linux64/studiorender.so bin/linux64/vphysics.so bin/linux64/vphysics_jolt.so \
	bin/linux64/engineaudio.so bin/linux64/soundemittersystem.so bin/linux64/scriptsystem.so bin/linux64/vscript.so \
	bin/linux64/localize.so bin/linux64/scenefilecache.so bin/linux64/vgui2.so \
	bin/linux64/video_services.so
P2CE_LAUNCH_MODULES ?= bin/linux64/p2ce bin/linux64/libmimalloc.so bin/linux64/libtier0.so \
	bin/linux64/libvstdlib.so bin/linux64/libSDL2-2.0.so.0 bin/linux64/launcher.so \
	bin/linux64/filesystem_stdio.so bin/linux64/engine.so $(P2CE_SYSTEMS)
P2CE_STEAM_API ?= $(CURDIR)/deps/steamworks-2024.8.0/steam_api.bundle/Contents/MacOS/libsteam_api.dylib
P2CE_APPID ?= 440000
P2CE_ARGS ?= -game p2ce -textmode -novid -nosound -shaderapi shaderapiempty
P2CE_CONTENT ?= $(subst depot_440005,depot_440002,$(P2CE_ROOT))
P2CE_TIMEOUT ?= 120
# -O0 by default: engine.so is 170 C files; -O1 takes about 2 GB and minutes per file.
P2CE_OPT ?= 0
.PHONY: p2ce-launch
p2ce-launch: $(X64_RUNTIME) runtime/x64/rt64.h src/main64.c tools/run64.py
	@if [ ! -d "$(P2CE_ROOT)/bin/linux64" ]; then echo "p2ce-launch: skipped: no P2CE depot at P2CE_ROOT=$(P2CE_ROOT)"; exit 0; fi; \
	set -e; $(PYTHON) tools/build64.py --out build/p2ce/lib --opt $(P2CE_OPT) --registry build/p2ce/registry.c \
		$(foreach m,$(P2CE_LAUNCH_MODULES),"$(P2CE_ROOT)/$(m)"); \
	$(CC) $(X64_CFLAGS) -Iruntime $(X64_RUNTIME) src/main64.c build/p2ce/registry.c \
		$$(ls build/p2ce/lib/*/lib*.a) $(X64_LIBS) -o build/p2ce/p2ce-run; \
	P2_STEAM_API_DYLIB="$(P2CE_STEAM_API)" P2_STEAM_APPID=$(P2CE_APPID) \
	$(PYTHON) tools/run64.py --depot "$(P2CE_ROOT)" --depot "$(P2CE_CONTENT)" --mirror build/p2ce/game --timeout $(P2CE_TIMEOUT) \
		build/p2ce/p2ce-run $(P2CE_ARGS)

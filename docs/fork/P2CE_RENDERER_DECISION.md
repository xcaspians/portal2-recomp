# P2CE: the renderer decision

Round 27, research only. Nothing in this page is built yet: **it waits for
Caspian's OK.**

## The problem

P2CE draws through `shaderapidx11.so`, which speaks **Direct3D 11**. On
Linux, Direct3D 11 isn't the system's: P2CE ships **DXVK**
(`libdxvk_d3d11.so.0`, `libdxvk_dxgi.so.0`), which turns Direct3D 11 into
**Vulkan**. On a Mac or an iPad there is no Vulkan either; there is
**Metal**. So something has to stand between `shaderapidx11.so` (lifted,
x86-64 code) and Metal.

Portal 2, Half-Life 2, Mel and L4D2 already have such a layer for
**Direct3D 9**: `mtld3d` (`deps/mtld3d-native`, about 360,000 lines of Rust
counting its Wine side), linked as `libd3d9metal`. It doesn't speak
Direct3D 11.

## Route (a): DXVK, then MoltenVK

Keep DXVK and add **MoltenVK** (Khronos's Vulkan-on-Metal library, free, used
by many shipping games, App Store compatible on iOS).

```
shaderapidx11.so (lifted) -> DXVK (D3D11 -> Vulkan) -> MoltenVK (Vulkan -> Metal)
shaders: DXBC --DXVK--> SPIR-V --SPIRV-Cross, inside MoltenVK--> Metal shading language
```

There are two ways to get DXVK:

- **(a1) Lift P2CE's own DXVK** like any other `.so`. Simplest on paper, but:
  - DXVK 2.x (what current P2CE builds ship) *requires* Vulkan features
    MoltenVK doesn't have: geometry shaders, transform feedback,
    `nullDescriptor` robustness, and some dynamic state. It stops at device
    creation. The Mac DXVK builds that work (the "DXVK-macOS" fork used under
    Wine) are DXVK 1.10 with those requirements patched out.
  - DXVK runs its own threads (a command-stream thread, pipeline compile
    workers). Lifted, they work (round D's threads), but shader compilation
    runs through lifted code: noticeably slower, and shader stutter is
    DXVK's weak spot already.
  - Every Vulkan call crosses from lifted x86-64 code to native arm64
    MoltenVK through the native-call trampoline (`runtime/x64/native64.c`),
    hundreds of calls per frame: workable, it's the Steam bridge's path, but
    each new struct-by-value or callback is bridge work.
- **(a2) Build DXVK natively for Apple arm64** ("dxvk-native", DXVK's own
  non-Windows build, with the macOS fork's patches), and call it from the
  lifted `shaderapidx11.so` through the same trampoline. The D3D11 interface
  is C++ vtables, which the Steam bridge already crosses (round 23). Shader
  translation and DXVK's threads then run as native code.

**Size:** (a1) 3 to 4 rounds to the first frame *if* the lifted DXVK accepts
MoltenVK's feature set, which it likely won't: count on (a2). (a2) is
**about 3 rounds**: one to build dxvk-native + MoltenVK for macOS arm64 and
bridge the D3D11 and DXGI entry points, one to the first clear and the
loading screen, one for the gaps (formats, queries, swapchain, resizing).

**Likely to break:**
- MoltenVK gaps: no geometry shaders, no transform feedback, a few texture
  formats and border colours missing. Source's D3D11 path mostly avoids
  geometry shaders; if P2CE's shaders use them, those effects go missing.
- DXVK's swapchain expects a window surface: on the Mac an SDL `CAMetalLayer`,
  which SDL already provides.
- Stutter from pipeline compilation the first time each effect appears
  (DXVK's state cache helps on later runs).

**iOS later:** MoltenVK runs on iOS and is App Store safe; dxvk-native builds
for arm64 iOS the same as for macOS. Memory: Vulkan objects on top of Metal
objects cost a little more (tens of MB, not hundreds). No code is written at
run time (iOS forbids it), and none needs to be.

**Next to the D3D9 bridge:** separate. Two renderers in the app, sharing
nothing but the window. Fine for Portal 2 and the others, which keep
`mtld3d` as it is.

## Route (b): a direct Direct3D 11 to Metal bridge

Write our own Direct3D 11 layer straight onto Metal, the way `mtld3d` does
for Direct3D 9.

```
shaderapidx11.so (lifted) -> our D3D11 (-> Metal)
shaders: DXBC (shader model 4/5) --our translator--> Metal shading language
```

**Size:** **6 to 10 rounds.**
- The Direct3D 11 device and context (resources, views, state objects, the
  immediate context's ~110 methods, maps and copies, queries): 2 to 3 rounds.
- A DXBC (shader model 4 and 5) to Metal translator: the biggest single piece,
  3 to 5 rounds. Shader model 4/5 is a different bytecode from the shader
  model 2/3 `mtld3d` translates, so its translator doesn't carry over;
  open-source references exist (DXVK's DXBC front end, SPIRV-Cross's Metal
  back end, the "DXMT" project's D3D11-on-Metal for Wine) but each needs
  adapting.
- Swapchain, formats, edge cases: 1 to 2 rounds.

**Likely to break:** whatever we haven't written yet; every missing method
or bytecode instruction is a black or missing effect until it is. Threads are
simpler than DXVK's (one context, our own command encoding).

**iOS later:** the best of the two: Metal directly, no extra layer, the same
memory budget habits as `mtld3d`, nothing else to ship.

**Next to the D3D9 bridge:** the natural place is beside `mtld3d`, reusing
its Metal plumbing (device and queue set-up, texture uploads, the frame
pacing and the TV output it already has). That sharing is the main long-term
argument for (b). The shader translator can't be shared.

## Recommendation

**Route (a2): DXVK built natively for Apple arm64, on MoltenVK.**

- It's the shortest way to pixels (about 3 rounds against 6 to 10), and
  P2CE's renderer was written against DXVK in the first place, so
  `shaderapidx11.so` sees the same D3D11 implementation it was tested with on
  Linux.
- Shader translation is solved by DXVK and SPIRV-Cross, mature code used by
  many games, instead of a new translator of our own.
- It carries over to iOS (MoltenVK and dxvk-native both build for it).
- It doesn't rule out (b): if MoltenVK's gaps or its overhead show up on the
  iPad, a direct bridge can replace it later behind the same D3D11 entry
  points, with the game running to compare against.

Not recommended: (a1), lifting P2CE's shipped DXVK. Its Vulkan requirements
likely exceed MoltenVK's, and its shader compiler would run as lifted code.

## What I need from you

1. **OK for route (a2)**, or pick (b).
2. **Licences:** DXVK (zlib) and MoltenVK (Apache 2.0) are fine to build and
   ship, but they'd be the first third-party graphics libraries built by
   `setup.sh` rather than downloaded prebuilt. OK to add them as sources
   fetched by `setup.sh`, like `mtld3d`?

## Round 28: route (a2) approved, stopped at a MoltenVK gap

Caspian approved (a2). Before writing any bridge, I built MoltenVK v1.4.2 from source for macOS (Apple M6,
Vulkan 1.3.357) and asked it directly which of DXVK's required features it has. DXVK is now v3.2: Vulkan 1.3
and about 55 required features. MoltenVK has all of them except these:

| Missing in MoltenVK 1.4.2 | DXVK 3.2 | Effect |
|---|---|---|
| `nullDescriptor` (VK_EXT_robustness2) | required | **Blocking.** DXVK 3.x writes an unbound slot as a null descriptor, and D3D11 games unbind resources all the time. DXVK marks it "effectively required for correctness". |
| `robustBufferAccess2` (same extension) | required | Relaxable on its own: out-of-range reads aren't zeroed. Comes with the line above. |
| `geometryShader` | required | Small: made optional the way the macOS fork does; geometry-shader effects go missing. |
| `shaderCullDistance` | required | Small: optional, same patch. |
| `depthClipEnable` (VK_EXT_depth_clip_enable) | required | Small: depth clipping stays on, so only states that turn it off draw wrong. |
| `transformFeedback` | optional with the fork's patch | Small: stream output effects go missing. |

The macOS fork (Gcenx/DXVK-macOS) is DXVK **1.10.3**. Its macOS changes are small: geometry shaders,
tessellation, cull distance and transform feedback become optional. It works on MoltenVK because DXVK 1.10
still bound dummy resources where 3.x uses null descriptors.

**Ways past the block (Caspian's call):**
1. **DXVK 1.10.3 with the macOS fork's patches, built natively.** 1.10 predates DXVK's own native build, so
   the native window-system and build changes (dxvk-native's 1.x branch) have to be carried over. About 1
   round; an old DXVK, but the one Macs already run under Wine.
2. **DXVK 3.2 with a dummy-resource fallback** written back into its descriptor code wherever
   `nullDescriptor` is assumed. Deep changes to current DXVK, to maintain on every update.
3. **Wait for MoltenVK** to add robustness2 null descriptors.
4. **DXMT** (native build: D3D11 straight to Metal, no Vulkan step). It has a native `libdxmt-native.dylib`
   with the D3D11/DXGI entry points, but its native build has no stable release I could confirm.

**Another prerequisite, whichever route:** P2CE hands DXVK an SDL window. Today SDL is P2CE's lifted Linux
SDL, which can't open a Mac window. The plan's round E (bridge SDL to the Mac's native SDL) comes before the
first clear.

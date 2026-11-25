# P2CE rounds 16 to 18 (plan pieces B, C, D): learning notes

Written 25 November 2026. Plain-English notes for Jacksen on the three
pieces of [P2CE_PLAN.md](P2CE_PLAN.md) done in fork rounds 16, 17 and 18.
Round 13's notes ([P2CE_ROUND13_NOTES.md](P2CE_ROUND13_NOTES.md)) explain
the basics (lifter, runtime, shims, registers); these build on them.

## Piece B: the whole instruction set, and whole libraries compiled

**What it was.** Round 13's lifter knew the common instructions. A game
library also uses rarer families, and every one the lifter doesn't know is
a "stop sign" (a trap) in the C. Piece B went through all 114 Linux
libraries and programs P2CE ships, counted what was still missing, and
filled it in:

- **SIMD instructions** ("single instruction, multiple data": one
  instruction works on 4 floats or 16 bytes at once; the engine uses them
  for maths and the video/audio libraries for pixels and samples). The
  newer families, called SSE3, SSSE3 and SSE4.1, are all lifted now.
- **x87**, the original 1980s floating-point unit, still used for
  `long double`. Like the 32-bit games, its numbers are kept as doubles.
- **MMX**, the first SIMD family (64-bit registers), used by the video
  decoder.
- **BMI**, a few bit-twiddling instructions.

Some families are deliberately left as traps: AVX, AVX-512, AES and a few
more. A program asks the processor what it supports (the CPUID
instruction) before using them, and the runtime's answer leaves them out,
so the program picks its plain version instead. The 32-bit games work the
same way.

Two speed-ups came with it. **Flags:** after most arithmetic the processor
records side facts (was the result zero? negative? did it overflow?). The
lifter used to save them at every function call. On 64-bit Linux the
rules say a function never relies on its caller's flags, so the lifter
now drops those saves unless it can see a flag being read after a call.
**Splitting:** a library becomes about 10 to 30 C files of 1.5 MB each,
compiled two at a time with a cache. A file that didn't change is never
compiled again.

**Numbers.** A full count over the depot (`make p2ce-coverage`): 147 files,
81.4 million instructions, of which 2,716 are not handled yet (99.997 %
handled; every file is above 99.7 %, and `engine.so`, `client.so` and
`server.so` are at 100 %). Be careful with that figure: 1.87 million of the
"handled" instructions are deliberate traps, almost all AVX code that only
runs on processors that report AVX (mostly in the audio, video and Qt
libraries). Before this round it was 96.8 % with nothing gated. `libtier0.so` (3,047 functions) and
`libvstdlib.so` (935) compile whole in about 15 seconds (`make p2ce-build`).

## Piece C: several libraries that talk to each other

**What it was.** A program is many libraries. Each one calls functions
of the others and runs some set-up code when it loads (its
**constructors**). On Linux the system's loader (`ld.so`) wires this up.
Our runtime now does the same job:

- **Loading in order.** Opening `filesystem_stdio.so` first opens the
  libraries it lists as needed, then fixes every reference to a function
  of another library to point at it (the **relocations**), then runs the
  constructors.
- **Thread-local storage** (TLS): variables with one copy per thread.
  Each library's share sits just below a per-thread pointer, as on Linux.
- **setjmp/longjmp**: a C way to jump back to an earlier point in the
  program, skipping the functions in between.
- **C++ exceptions**: `throw` in one function, `catch` in another further
  up. This sounded like the hardest part, and it went in cleanly: P2CE
  ships its own C++ runtime inside `libmimalloc.so`. That runtime walks
  back up the stack using tables the compiler left in each library, then
  jumps into the right `catch` block. The lifter now knows where those
  `catch` blocks (**landing pads**) are, and each lifted function that has
  one is ready to resume there.

**Milestone, reached.** `make p2ce-modules-check` loads `libmimalloc.so`,
`libtier0.so`, `libvstdlib.so` and `filesystem_stdio.so`, lifted whole,
and runs all their constructors. Then `CommandLine()->CheckParm("-game")`
returns `p2ce`, the file system hands out its `VFileSystem018` interface,
and a C++ exception thrown two functions down is caught. 15 checks.

## Piece D: the system bridge

**What it was.** A program talks to the operating system all the time:
open a file, start a thread, read the clock, ask how much memory there is.
On Linux it does this through the C library (glibc), and sometimes
directly with a `syscall` instruction. Each of these requests has to be
answered by a **shim** that translates it to macOS. The 64-bit shims are
simpler than the 32-bit ones because both systems agree on the size of
numbers and pointers, but many details still differ and each one matters:

- **Different structure layouts.** A file's information (`struct stat`),
  a directory entry, a time value with microseconds: same idea, fields in
  different places. The shims copy them field by field.
- **Different numbers.** Error codes (Linux's "try again" is 11, Apple's
  is 35), file-opening flags, clock names, memory-mapping flags.
- **Threads.** A new game thread is a real Mac thread, with its own copy
  of the processor state, its own stack and its own thread-local storage.
- **Locks.** Linux's lock objects are smaller than Apple's, so each guest
  lock holds a pointer to a real Mac lock, created the first time it is
  used.
- **/proc.** Linux exposes system information as fake files under `/proc`
  (processor list, memory size, the program's own path). The runtime
  makes those files up from the Mac's real values.
- **Starting a program.** On Linux the kernel lays out the program's first
  stack (its arguments, environment and a few facts about the system) and
  the loader runs every library's set-up code before `main`. The runtime
  does the same, then jumps to the program's real first instruction.
- **Never touching Steam's files.** The game runs in a "mirror" of its
  folder made of shortcuts (symlinks) to Steam's download. When the game
  writes a file, the shortcut is first replaced by a private copy, so
  Steam's own files are never changed.

**Milestone, not reached, and why.** The plan asked for the engine to
start and print its console banner. `make p2ce-launch` gets this far:
P2CE's own `p2ce` program starts like on Linux, loads `launcher.so` and
runs it, and the launcher loads the file system. The file system then
asks Steam for its "context". There is no Linux Steam client, so it stops
with `Failed to initialize filesystem Steam API context`, before
`engine.so` is opened. That is the right behaviour: the real game on
Linux does the same without Steam. The project's rule is to go through
Valve's own Steam library on the Mac, never to fake Steam, and that
bridge is round E of the plan. So the next step is to move the Steam part
of round E ahead: send the lifted libraries' Steam calls to Valve's
`libsteam_api.dylib`. Most Steam functions take and return plain C
values, which pass straight through on 64-bit. The C++ interface objects
need small adapters ("thunks"), like the 32-bit bridge already has. With
that, the engine is the next thing the launcher opens.

**What it took.** Eight lifted libraries (the program itself, the C++
runtime and memory allocator, tier0, vstdlib, SDL2, Steam's API library,
the launcher and the file system) run their start-up code and talk to
each other. With `engine.so`, which is lifted and compiled too, ready for
when the launcher gets that far, that is 61,000 functions, 3.6 million
instructions and 400 MB of generated C (`engine.so` alone: 41,393
functions, 2.3 million instructions, 172 C files), with no instruction
left as a trap.

# P2CE round 13: learning notes

Written 24 November 2026. Plain-English notes for Caspian on the first round
of Portal 2: Community Edition (P2CE): what is different about 64-bit code,
why the recompiler had to change, what this round built and what comes next.
The roadmap is [P2CE_PLAN.md](P2CE_PLAN.md).

## First, a reminder: what the recompiler does

A game for Linux is a pile of **machine code**: the numbered instructions an
Intel or AMD processor runs directly ("add these two numbers", "jump over
there if the result was zero"). A Mac with Apple silicon or an iPhone can't
run those instructions; it speaks a different machine language (ARM).

This project doesn't emulate the Intel processor while the game runs. Ahead
of time, a Python tool (the **lifter**) reads every instruction of the
game's files and writes an equivalent line of C, the programming language.
Apple's compiler then turns that C into ARM code. The result runs at close
to native speed. Whatever the game asks of Linux (open a file, start a
thread, draw on screen) is answered by the **runtime**, a library of C code
that translates those requests to macOS or iOS. The runtime's small
translators are called **shims**.

Portal 2, Portal, Half-Life 2, Mel and L4D2 were all built for **32-bit
Intel** (also called i386 or x86). P2CE is built for **64-bit Intel**
(x86-64). Same family, but different enough that the lifter can't read it.

## What's different about 64-bit code

1. **Bigger registers, and twice as many.** A **register** is a tiny named
   slot inside the processor where it keeps the numbers it is working on.
   32-bit Intel has 8 general registers of 32 bits (`eax`, `ebx`...).
   64-bit Intel has 16 of 64 bits (`rax`, `rbx`..., plus `r8` to `r15`).
   The old names still exist as the lower halves, with a twist: writing the
   lower 32 bits of a register *wipes* the upper 32 bits, while writing the
   lower 8 or 16 bits leaves the rest alone. The lifter has to copy that
   rule exactly, or numbers quietly go wrong.
2. **A new prefix byte (REX).** To reach the 8 new registers and the 64-bit
   sizes, Intel added a prefix byte in front of instructions. The same bytes
   can mean a different instruction in 32-bit and 64-bit mode, so a
   decoder for one mode misreads the other.
3. **Addresses relative to the current instruction (RIP-relative).** In
   32-bit code, a game library finds its own global variables through a
   helper register set up at the start of each function. In 64-bit code,
   an instruction says "the variable 0x12e2c5 bytes after me". The lifter
   turns that into "the library's start address plus a fixed offset".
4. **A different calling convention.** The **calling convention** is the
   agreement on how a function receives its arguments and returns its
   result. On 32-bit Linux, arguments are pushed on the **stack** (a scratch
   area of memory). On 64-bit Linux ("System V AMD64"), the first six go in
   registers (`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`), decimals go in
   `xmm0` to `xmm7`, and only extra ones go on the stack. Every shim has to
   read its arguments from the new places.
5. **Pointers are 8 bytes.** A **pointer** is a memory address stored as a
   number. 32-bit code stores them in 4 bytes, so every structure the game
   shares with the system has a different shape in 64-bit code.

## Why the lifter had to change, and the good news

The 32-bit lifter (`tools/lift.py`) is built around 8 registers of 32 bits
and a 4 GB guest memory placed at an offset inside the Mac's memory: every
memory access in the generated C adds that offset. None of that fits 64-bit
code, so this round wrote a **separate 64-bit lifter** (`tools/lift64.py`)
and **runtime** (`runtime/x64/`), and left the 32-bit ones untouched so the
five games that work today can't be broken by this work.

The good news is the memory. Linux on 64-bit Intel and macOS/iOS on Apple
silicon agree on the size of numbers and pointers and on the byte order
(both are "LP64 little-endian"). So the game's memory can simply *be* the
Mac's memory: a pointer in the game is a real pointer on the Mac. The
lifted library is placed wherever macOS has room, and the addresses inside
it are fixed up as it loads ("relocations"). That makes calling Mac
libraries from the game much simpler than with the 32-bit games, where
every structure had to be copied and reshaped.

## What this round built

- **A roadmap** (`docs/fork/P2CE_PLAN.md`): rounds A to K, each with a size
  and something you can check yourself.
- **An ELF64 reader** (`tools/elfprep64.py`). Linux programs and libraries
  are stored in a file format called ELF; 64-bit ones use its 64-bit
  variant. The reader writes the library's memory image and a description
  of it (functions, imports, the fix-ups to apply at load).
- **The first 64-bit lifter** (`tools/lift64.py`). Apple's `objdump` tool
  decodes the machine code (it already understands REX and the other
  prefixes); the lifter turns each instruction into C. This first slice
  covers the 16 registers with their width rules, RIP-relative memory, the
  System V calls, jumps and calls (including "switch" jump tables and calls
  through the import table), arithmetic and logic with all their condition
  flags, multiply and divide up to 128-bit results, bit operations,
  memory-block instructions, the atomic instructions threads use, and the
  common decimal (SSE2) instructions. An instruction it doesn't know yet
  becomes a stop sign ("trap") in the C and is counted, so a library always
  compiles and the report says exactly what is missing.
- **A 64-bit runtime** (`runtime/x64/`): it loads a lifted library, applies
  its fix-ups, finds the right C function for "call whatever this pointer
  points to", and runs the first shims (`memset`, `strlen`, `malloc`,
  `clock_gettime`...).
- **A test in `make check`** (`build/x64-check`). This Mac has no Linux
  linker (the tool that glues compiled pieces into a library), so a tiny one
  was written for the test (`tests/x64_link.py`). Small 64-bit test
  functions are assembled, linked, lifted and run, and 62 answers are
  compared with the values from Intel's manuals. It takes a few seconds and
  needs no game files.
- **The real milestone.** `P2_P2CE_TESTS=1 make check` (or
  `make p2ce-tier0-check`) takes P2CE's own `libtier0.so` (its basic
  utility library) from your Steam folder, lifts it and calls nine of its
  functions on the Mac: string copy and compare, a timer, atomic exchanges,
  a memory clear handed on to the Mac's own `memset`, a settings flag. All 12 checks
  pass. Over the whole library, the lifter handles 238,670 of its 238,816
  instructions (99.94 %); what's missing is a handful of rare instructions
  (system calls made directly, Intel's shadow-stack security instructions,
  some packed-integer and old-style x87 decimal instructions).

## What's next

"Handled" means the lifter writes C for it, not that every line has been
proven right: the test programs cover the common cases, and more tests come
with each round. Next round (B in the plan) finishes the instruction set,
makes the generated code faster (dropping condition flags nobody reads, as
the 32-bit lifter does) and compiles whole libraries. Then round C loads
several libraries together and runs their start-up code. About a third of
`libtier0.so`'s code is not reached yet from the entry points the lifter
knows (it is still checked by the instruction count above); most of it looks
like C++ exception handlers ("landing pads", reached only when an error is
thrown), which that round handles. After that, the system bridge for
64-bit (files, threads, time), then the window, then Direct3D 11 through
DXVK and MoltenVK, V8 and Panorama, and finally iOS.

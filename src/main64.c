// Host entry point of a lifted x86-64 Linux program (P2CE, docs/fork/P2CE_PLAN.md).
// The modules built by tools/build64.py register themselves through
// rt64_register_built() (generated next to them); the program module is
// RT64_PROGRAM (a compile-time symbol, m_p2ce for P2CE). tools/run64.py
// starts this binary in a mirror of the game folder and sets:
//   RT64_EXE             the program's Linux path (readlink /proc/self/exe, argv[0])
//   RT64_COPY_ON_WRITE   files written through the mirror's symlinks become private copies
// The i386 games use src/main.c; this file is not part of their build.
#include "x64/rt64.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef RT64_PROGRAM
#define RT64_PROGRAM m_p2ce
#endif

extern const RT64Module RT64_PROGRAM;
void rt64_register_built(void);
extern char **environ;

int main(int argc, char **argv) {
    rt64_register_built();
    const char *exe = getenv("RT64_EXE");
    if (exe) argv[0] = (char *)exe;
    setvbuf(stdout, NULL, _IOLBF, 0);
    return rt64_run_program(&RT64_PROGRAM, argc, argv, environ);
}

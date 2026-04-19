#include <stdio.h>

/**
 * Linker Fix for Windows (LLVM-MinGW / UCRT)
 * 
 * Some pre-compiled libraries in SFML extlibs (like libFLAC.a) were built 
 * against the legacy msvcrt.dll and expect the __iob_func symbol to access 
 * stdin, stdout, and stderr. 
 * 
 * Since modern LLVM-MinGW uses UCRT, it defines _iob as a macro to __iob_func().
 * We provide this shim to satisfy the linker, returning a local array of FILE pointers.
 */

#ifdef __cplusplus
extern "C" {
#endif

// Guard against recursive macro expansion from stdio.h
#undef _iob

FILE * __cdecl __iob_func(void) {
    static FILE iob_fix_array[3];
    iob_fix_array[0] = *stdin;
    iob_fix_array[1] = *stdout;
    iob_fix_array[2] = *stderr;
    return iob_fix_array;
}

#ifdef __cplusplus
}
#endif

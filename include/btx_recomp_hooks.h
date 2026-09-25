#ifndef __BTX_RECOMP_HOOKS_H__
#define __BTX_RECOMP_HOOKS_H__

// Force-included into the generated code in RecompiledFuncs/ (see CMakeLists.txt). It adds what
// stock N64Recomp's output lacks for this game: the functions the [[patches.hook]] text in
// battletanx.us.rev0.toml calls, declarations for the libultra functions the config marks as
// reimplemented, and memory macros that accept KSEG1 addresses.

#include <stdint.h>
#include "recomp.h"

// Defined in src/game/stock_runtime_compat.cpp.
void yield_self_1ms_recomp(uint8_t* rdram, recomp_context* ctx);

// Provided by librecomp. The config lists these under ignored + renamed, which makes the
// generated code call them but does not put a declaration in funcs.h.
void osSiRawStartDma_recomp(uint8_t* rdram, recomp_context* ctx);
void osSpSetStatus_recomp(uint8_t* rdram, recomp_context* ctx);

// The game reads and writes the uncached KSEG1 mirror (0xA0000000 and up) as well as KSEG0.
// recomp.h subtracts 0xFFFFFFFF80000000, which is only right for KSEG0, so mask down to the
// physical address instead.
#undef MEM_W
#undef MEM_H
#undef MEM_B
#undef MEM_HU
#undef MEM_BU
#undef SD

#define MEM_W(offset, reg) \
    (*(int32_t*)(rdram + (((uint32_t)((reg) + (offset))) & 0x1FFFFFFF)))

#define MEM_H(offset, reg) \
    (*(int16_t*)(rdram + ((((uint32_t)((reg) + (offset))) ^ 2) & 0x1FFFFFFF)))

#define MEM_B(offset, reg) \
    (*(int8_t*)(rdram + ((((uint32_t)((reg) + (offset))) ^ 3) & 0x1FFFFFFF)))

#define MEM_HU(offset, reg) \
    (*(uint16_t*)(rdram + ((((uint32_t)((reg) + (offset))) ^ 2) & 0x1FFFFFFF)))

#define MEM_BU(offset, reg) \
    (*(uint8_t*)(rdram + ((((uint32_t)((reg) + (offset))) ^ 3) & 0x1FFFFFFF)))

#define SD(val, offset, reg) { \
    *(uint32_t*)(rdram + ((((uint32_t)((reg) + (offset) + 4))) & 0x1FFFFFFF)) = (uint32_t)((gpr)(val) >> 0); \
    *(uint32_t*)(rdram + ((((uint32_t)((reg) + (offset) + 0))) & 0x1FFFFFFF)) = (uint32_t)((gpr)(val) >> 32); \
}

#endif

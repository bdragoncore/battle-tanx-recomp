#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include "ultramodern/ultramodern.hpp"
#include "recomp.h"

// Stubs for manual patch symbols referenced by RecompiledPatches/recomp_overlays.inl
// but not provided by the clean N64ModernRuntime e613aff (they were added in later
// BattleTanxRecomp runtime and in newer N64Recomp symbol lists). The current
// battletanx_boot patches only use recomp_run_ui_callbacks / osCreateMesgQueue /
// osRecvMesg etc., so these are never called at runtime – they just need to
// exist for the linker.

extern "C" {

void malloc_recomp(uint8_t* rdram, recomp_context* ctx) {
    // a0 = size in r4, return pointer in r2 (MIPS o32)
    int32_t size = (int32_t)ctx->r4;
    void* p = size > 0 ? malloc(size) : nullptr;
    // Return as 32-bit KSEG0 address? Patches expect rdram-mapped pointer.
    // For now return 0 (never used by current patches).
    ctx->r2 = p ? (uint32_t)(uintptr_t)p : 0;
}

void free_recomp(uint8_t* rdram, recomp_context* ctx) {
    void* p = (void*)(uintptr_t)(uint32_t)ctx->r4;
    if (p) free(p);
}

void realloc_recomp(uint8_t* rdram, recomp_context* ctx) {
    void* p = (void*)(uintptr_t)(uint32_t)ctx->r4;
    int32_t size = (int32_t)ctx->r5;
    void* np = realloc(p, size > 0 ? size : 0);
    ctx->r2 = np ? (uint32_t)(uintptr_t)np : 0;
}

void __cosf_recomp(uint8_t* rdram, recomp_context* ctx) {
    // Stub - not used by current patches. Provide a no-op so linker is satisfied.
    // If called, return 1.0f (cos 0) to avoid NaN.
    ctx->r2 = 0x3F800000; // 1.0f bits
}

void bcopy_recomp(uint8_t* rdram, recomp_context* ctx) {
    // void bcopy(void *src, void *dest, size_t n)
    uint32_t src = (uint32_t)ctx->r4;
    uint32_t dst = (uint32_t)ctx->r5;
    int32_t n = (int32_t)ctx->r6;
    if (n > 0) {
        // src/dst are RDRAM virtual addresses (0x80000000+offset). Translate via rdram.
        // Use MEM_W helpers would be ideal, but for stub just memmove raw rdram offsets
        // if they look like KSEG0.
        auto to_offset = [](uint32_t v) -> uint32_t { return v & 0x1FFFFFFF; };
        uint32_t so = to_offset(src);
        uint32_t doff = to_offset(dst);
        if (so + (uint32_t)n <= 0x800000 && doff + (uint32_t)n <= 0x800000) {
            memmove(rdram + doff, rdram + so, n);
        }
    }
}

void osAiSetFrequency_recomp(uint8_t* rdram, recomp_context* ctx);

// func_80110490 (0x80110490) is BattleTanx's own libultra osAiSetFrequency, recompiled as game
// code under an unrecognized name: it divides the VI clock and pokes the AI rate registers,
// so the runtime's osAiSetFrequency_recomp hook never ran and the host audio bridge kept
// resampling the game's 22050 Hz output as if it were 48000 Hz (draining the ring ~2.2x too
// fast -> constant underruns -> static). This strong definition overrides the weak
// recompiled one and routes the call into the runtime: a0 = requested rate, v0 = rate set.
// The game only uses v0 to size the audio buffer (2*freq/60 rounded up to 16 -> 736 either way).
void func_80110490(uint8_t* rdram, recomp_context* ctx) {
    osAiSetFrequency_recomp(rdram, ctx);
}

}

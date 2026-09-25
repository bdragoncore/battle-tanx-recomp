#include "patches.h"
#include "ui_funcs.h"
#include "patch_helpers.h"

// Boot patches for BattleTanx on the stock N64ModernRuntime.
//
// Stock only switches game threads when one blocks or yields, and only then delivers VI/SP/SI
// messages to a game queue. BattleTanx waits on some flags with bare busy loops, so the threads
// that would clear them never run. These patches add the missing yields. Loops inside larger
// functions are patched with [[patches.hook]] entries in battletanx.us.rev0.toml instead.

// The controller scan reads one 4-byte word per channel from 0x802DF390. On hardware the PIF fills
// them in through a DMA the game starts itself. The runtime has no PIF model, so every channel
// looks alike and player 0 lands in slot 3 (D_802E17A8 == 3). Write what the console returns with
// one pad in channel 0: 00 05 01 00 | 00 00 00 08 | 00 00 00 08 | 00 00 00 08
static void battletanx_synthesize_si_scan_response(void) {
    // Patches run as recompiled code, so KSEG0 addresses can be used as plain pointers.
    volatile unsigned char *p = (volatile unsigned char *)0x802DF390;
    const unsigned char si_scan_response[16] = {
        0x00, 0x05, 0x01, 0x00,  0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x00, 0x08,  0x00, 0x00, 0x00, 0x08,
    };
    for (int i = 0; i < 16; i++) p[i] = si_scan_response[i];
}

// @recomp Patched osContInit (0x80110FE0). The game only needs channel 0 to report a controller,
// so skip the libultra query and fill in the result directly.
RECOMP_PATCH s32 osContInit(OSMesgQueue *mq, u8 *bitpattern, OSContStatus *status) {
    // One controller on channel 0, nothing on the other channels.
    if (bitpattern) *bitpattern = 0x01;
    if (status) {
        status[0].type = CONT_TYPE_NORMAL;
        status[0].status = 0;
        status[0].errno = 0;
        for (int i = 1; i < MAXCONTROLLERS; i++) {
            status[i].type = 0;
            status[i].status = 0;
            status[i].errno = CONT_NO_RESPONSE_ERROR;
        }
    }
    battletanx_synthesize_si_scan_response();
    (void)mq;
    return 0;
}

// @recomp Run the recompui per-frame callbacks from the game's VI swap wrapper (0x80079530).
// osViSwapBuffer would be the obvious hook, but the runtime implements it natively and a
// RECOMP_PATCH can't replace it, so hook the wrapper that calls it once per frame instead.
// The two VI calls go through their _recomp entry points: the generated patch code passes its
// arguments in the guest registers, which is what those read, whereas the plain names are the
// host functions that take real C arguments.
extern s32 func_8007F204(s32);
DECLARE_FUNC(void, osViBlack_recomp, u8 active);
DECLARE_FUNC(void, osViSwapBuffer_recomp, void *frameBuf);
extern void battletanx_apply_cheats(void);  // patches/cheats.c
RECOMP_PATCH void func_80079530(void *arg0) {
    recomp_run_ui_callbacks();
    // Apply the cheat flags set from the Cheats tab.
    battletanx_apply_cheats();

    // The rest is the original function.
    u16 unk1F0 = *(volatile u16 *)((volatile u8 *)arg0 + 0x1F0);
    u16 unk1EC = *(volatile u16 *)((volatile u8 *)arg0 + 0x1EC);
    if (unk1F0 < unk1EC) {
        return;
    }
    s32 fb = func_8007F204(1);
    if (fb == 0) {
        return;
    }
    if (*(volatile u16 *)((volatile u8 *)arg0 + 0x1F2) != 0) {
        osViBlack_recomp(0);
        *(volatile u16 *)((volatile u8 *)arg0 + 0x1F2) = 0;
    }
    osViSwapBuffer_recomp((void *)(u32)fb);
    *(volatile u16 *)((volatile u8 *)arg0 + 0x1F0) = 0;
}

// Unused placeholder export.
RECOMP_EXPORT void battletanx_boot_phase1_ready(void) {}

// @recomp Patched to yield while waiting for the RESET thread to clear the busy flag at
// [0x802E17D0]. The original spins without yielding, so RESET never runs.
#include "scheduler_funcs.h"
RECOMP_PATCH void func_800A8EE0(void) {
    while (*(volatile u32 *)0x802E17D0 != 0) {
        yield_self_1ms_recomp();
    }
}

// @recomp Patched to yield in DEFRAG's boot gate (0x80079064). The
// gate waits for the VI manager's state block at 0x8014E0D0 to show no work pending and a retrace
// counter of at least 5. Only the VI manager thread advances the counter, and it doesn't run unless
// this thread yields.
RECOMP_PATCH void func_80079064(void) {
    for (;;) {
        u16 busy = *(volatile u16 *)0x8014E2C4;
        u32 flag = *(volatile u32 *)0x8014E2CC;
        u16 counter = *(volatile u16 *)0x8014E2C0;
        if (busy == 0 && flag == 0 && counter >= 5) {
            return;
        }
        yield_self_1ms_recomp();
    }
}

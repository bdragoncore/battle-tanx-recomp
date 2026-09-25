#include <cstdio>
#include <cstring>

#include "recomp.h"
#include "librecomp/game.hpp"
#include "librecomp/rsp.hpp"
#include "ultramodern/ultramodern.hpp"

// Game-side replacements for runtime behaviour that only the forked N64ModernRuntime provided,
// so BattleTanx can run on the stock runtime.

// ------------------------------------------------------------------------------------------
// yield_self_1ms_recomp: stock defines yield_self_1ms (scheduling.cpp) but no recomp-callable
// wrapper. RECOMP_PATCH func_800A8EE0 and the stale-register spin patches call this.
extern "C" void yield_self_1ms(uint8_t* rdram);

extern "C" void yield_self_1ms_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)ctx;
    yield_self_1ms(rdram);
}

// ------------------------------------------------------------------------------------------
// Boot entry. The game's entry (0x80071000) sets $sp, clears BSS, calls its boot function and
// then hits `break 1, 2` - unreachable on hardware, but in the recomp the boot function returns
// once the game threads are running, and the stock runtime's do_break exits the process. Run the
// same steps without the trailing break. Registered as the GameEntry entrypoint in main.cpp.
extern "C" void func_800779CC(uint8_t* rdram, recomp_context* ctx);

extern "C" void btx_entrypoint(uint8_t* rdram, recomp_context* ctx) {
    ctx->r29 = (gpr)(int32_t)(0x80150000 - 0x1F38);            // lui/addiu $sp
    const uint32_t bss_start = 0x80147040, bss_end = 0x803C8880;  // $t0 .. $t1
    memset(rdram + (bss_start & 0x1FFFFFFF), 0, bss_end - bss_start);
    func_800779CC(rdram, ctx);
}

// ------------------------------------------------------------------------------------------
// func_80077200 (rsp_cop0_set_status, 0x80077200): `mtc0 $a0, Status; jr $ra`. The stock
// runtime's cop0_status_write aborts on any changed bit other than FR. Those bits have no
// meaning in the recomp, so store them directly and let the runtime handle only FR.
extern "C" void cop0_status_write(recomp_context* ctx, gpr value);

extern "C" void func_80077200(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    const uint32_t fr_bit = 0x04000000;
    uint32_t value = (uint32_t)ctx->r4;
    ctx->status_reg = (ctx->status_reg & fr_bit) | (value & ~fr_bit);
    cop0_status_write(ctx, ctx->r4);
}

// ------------------------------------------------------------------------------------------
// SP_STATUS and DPC register storage for the recompiled microcode (see include/rsp_stock_compat.hpp).
uint32_t rsp_sp_status = 0;
uint32_t dpc_start_reg = 0;
uint32_t dpc_end_reg = 0;
uint32_t dpc_last_start = 0;
uint32_t dpc_last_end = 0;
bool dpc_last_valid = false;

// Same SET_/CLR_ decoding as rsp_stock_compat.hpp's set_sp_status (kept local: that header is
// only force-included into rsp/*.cpp).
static void btx_set_sp_status(uint32_t value) {
    static const uint32_t set_values[9] = { 0x0002, 0x0008, 0x0020, 0x0080, 0x0200, 0x0800, 0x2000, 0x8000, 0x020000 };
    static const uint32_t clear_values[9] = { 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0400, 0x1000, 0x4000, 0x010000 };
    static const uint32_t flag_bits[9] = { 0x00000001, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000 };
    for (int i = 0; i < 9; i++) {
        if (value & set_values[i]) {
            rsp_sp_status |= flag_bits[i];
        }
        else if (value & clear_values[i]) {
            rsp_sp_status &= ~flag_bits[i];
        }
    }
    if (value & 0x200000) rsp_sp_status |= 0x00000002;
    if (value & 0x100000) rsp_sp_status &= ~0x00000002;
    if (value & 0x800000) rsp_sp_status |= 0x00000004;
    if (value & 0x400000) rsp_sp_status &= ~0x00000004;
}

// osSpSetStatus is in N64Recomp's reimplemented list but the stock runtime doesn't define it.
// The recompiled audio ucode polls SP_STATUS bit 0x80, so reflect the game's writes there.
extern "C" void osSpSetStatus_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    btx_set_sp_status((uint32_t)ctx->r4);
}

// The game inlines SP_STATUS reads; report the RSP idle.
extern "C" void osSpGetStatus_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = 0;
}

// ------------------------------------------------------------------------------------------
// SI register access: in N64Recomp's reimplemented list, absent from the stock runtime. There
// is no PIF RAM model, so the SI is always idle, reads return zero and writes are ignored.
extern "C" void osSiGetStatus_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = 0;
}

extern "C" void osSiReadIo_recomp(uint8_t* rdram, recomp_context* ctx) {
    // a0 = SI/PIF address, a1 = destination pointer in RDRAM.
    *(uint32_t*)(rdram + (((uint32_t)ctx->r5) & 0x1FFFFFFF)) = 0;
    ctx->r2 = 0;
}

extern "C" void osSiWriteIo_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = 0;
}

// ------------------------------------------------------------------------------------------
// osSiRawStartDma_recomp: the stock runtime has no SI DMA / PIF model. Synthesize the PIF
// responses the game's controller code expects and complete the DMA on the queues it waits on.

// Controller-identification response, as the game's controller code expects it.
static const uint8_t controller_id_response[64] = {
    0xFF,0x23,0x01,0x03,0x02,0xF8,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,
    0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,
    0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,
    0x00,0x03,0x00,0x03,0x00,0x03,0xD1,0x21,0xFE,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Button-read response: channel 0 present, no buttons, stick centred.
static const uint8_t button_read_response[64] = {
    0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,
};

extern "C" void osSiRawStartDma_recomp(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = 0;
    uint32_t pif = ((uint32_t)ctx->r5) & 0x1FFFFFFF;

    // Direction 1 (RDRAM -> PIF) sends a command; direction 0 (PIF -> RDRAM) reads the response.
    static bool last_was_id_cmd = false;
    if (ctx->r4 == 1) {
        last_was_id_cmd = (rdram[pif + 1] != 0xFF);
    }
    else {
        memcpy(rdram + pif, last_was_id_cmd ? controller_id_response : button_read_response, 64);
        // osContGetReadData returns early unless the controller count (0x803C4DC1) is set.
        uint32_t dc1_phys = ((uint32_t)0x803C4DC1) & 0x1FFFFFFF;
        if (rdram[dc1_phys] == 0) {
            rdram[dc1_phys] = 4;
        }
    }

    // Completion: the controller worker queues and the SI event queue.
    ultramodern::enqueue_external_message(0x802DF5D0, 0, false, false);
    ultramodern::enqueue_external_message(0x802DF5B0, 0, false, false);
    ultramodern::enqueue_external_message(0x802DF360, 0, false, false);
}

// ------------------------------------------------------------------------------------------
// RSP task preparation (called from get_rsp_microcode in main.cpp, which the stock runtime's
// run_task invokes before it loads the task into DMEM).

static uint8_t* btx_rdram = nullptr;

void btx_set_rdram(uint8_t* rdram) {
    btx_rdram = rdram;
}

// Per task-thread state handed from get_rsp_microcode to the ucode wrapper (both run on the
// thread executing recomp::rsp::run_task).
static thread_local RspUcodeFunc* pending_ucode = nullptr;
static thread_local uint32_t preserved_start = 0xF80;
static thread_local uint8_t preserved_dmem[0xF80];

static RspExitReason btx_skip_task(uint8_t* rdram, uint32_t ucode_addr) {
    (void)rdram;
    (void)ucode_addr;
    return RspExitReason::Broke;
}

static RspExitReason btx_run_wrapped_task(uint8_t* rdram, uint32_t ucode_addr) {
    // Stock run_task always loads 0xF80 bytes of ucode data; BattleTanx's tasks carry 0x800.
    // Undo the over-read so the ucode's own working state past the real data stays intact.
    if (preserved_start < 0xF80) {
        memcpy(dmem + preserved_start, preserved_dmem + preserved_start, 0xF80 - preserved_start);
    }
    return pending_ucode(rdram, ucode_addr);
}

RspUcodeFunc* btx_wrap_rsp_microcode(const OSTask* task, RspUcodeFunc* ucode) {
    if (ucode == nullptr) {
        return nullptr;
    }
    // Empty tasks (no data): nothing to run, report success so the game's task cycle continues.
    if (task->t.data_ptr == 0 || task->t.data_size == 0) {
        return btx_skip_task;
    }

    // The game's task load used to DMA the ucode pieces into RDRAM; replicate those copies.
    if (btx_rdram != nullptr) {
        auto rom = recomp::get_rom();
        uint32_t tdest = ((uint32_t)task->t.ucode) & 0x1FFFFFFF;
        uint32_t ddest = ((uint32_t)task->t.ucode_data) & 0x1FFFFFFF;
        if (task->t.type == M_AUDTASK) {
            if (rom.size() > 0x9F160 + 0xF80) {
                memcpy(btx_rdram + tdest, rom.data() + 0x9F160, 0xF80);
            }
        }
        else if (task->t.type == M_GFXTASK) {
            if (rom.size() > 0xD0A00 + 0x800) {
                memcpy(btx_rdram + tdest, rom.data() + 0xCF5D0, 0x1430);
                memcpy(btx_rdram + ddest, rom.data() + 0xD0A00, 0x800);
            }
        }
    }

    uint32_t data_len = task->t.ucode_data_size != 0 ? task->t.ucode_data_size : 0xF80;
    preserved_start = data_len < 0xF80 ? data_len : 0xF80;
    if (preserved_start < 0xF80) {
        memcpy(preserved_dmem + preserved_start, dmem + preserved_start, 0xF80 - preserved_start);
    }
    pending_ucode = ucode;
    return btx_run_wrapped_task;
}

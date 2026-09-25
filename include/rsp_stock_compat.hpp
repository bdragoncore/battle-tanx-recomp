#ifndef __RSP_STOCK_COMPAT_HPP__
#define __RSP_STOCK_COMPAT_HPP__

// Force-included (-include) into the recompiled RSP microcode (rsp/*.cpp) so BattleTanx builds
// against the stock N64ModernRuntime, which has none of these. Storage lives in
// src/game/stock_runtime_compat.cpp.

#include "librecomp/rsp.hpp"

// SP_STATUS as seen by the microcode. The BattleTanx audio ucode polls bit 0x80 as its
// task-end/halt test; the game's CPU-side osSpSetStatus writes reach it through
// osSpSetStatus_recomp (stock_runtime_compat.cpp).
extern uint32_t rsp_sp_status;

static inline uint32_t get_sp_status() {
    return rsp_sp_status;
}

static inline void set_sp_status(uint32_t value) {
    // SET_/CLR_ bit pairs (libultra layout): INTR -> bit 0, INTR_BREAK -> bit 6, SIGn -> bit 7+n.
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
    // HALT (SET 0x200000 / CLR 0x100000) and BROKE (SET 0x800000 / CLR 0x400000).
    if (value & 0x200000) rsp_sp_status |= 0x00000002;
    if (value & 0x100000) rsp_sp_status &= ~0x00000002;
    if (value & 0x800000) rsp_sp_status |= 0x00000004;
    if (value & 0x400000) rsp_sp_status &= ~0x00000004;
}

// DPC register seam used by the F3DEX microcode (rsp/f3dex.cpp). The "RDP" behind it is always
// idle and caught up, so none of the ucode's wait loops spin; the last DPC_START/DPC_END span is
// latched for the optional LLE path.
extern uint32_t dpc_start_reg;
extern uint32_t dpc_end_reg;
extern uint32_t dpc_last_start;
extern uint32_t dpc_last_end;
extern bool dpc_last_valid;

static inline uint32_t get_dpc_status() { return 0; }
static inline uint32_t get_dpc_current() { return 0x7FFFFFFFu; }
static inline uint32_t get_dpc_end() { return dpc_end_reg; }
static inline void set_dpc_status(uint32_t value) { (void)value; }
static inline void set_dpc_start(uint32_t value) { dpc_start_reg = value; }

static inline void set_dpc_end(uint32_t value) {
    dpc_end_reg = value;
    dpc_last_start = dpc_start_reg;
    dpc_last_end = value;
    dpc_last_valid = (value > dpc_start_reg);
}

// Bounds-checked DMA. The audio ucode can issue DMAs from bogus addresses while its task data is
// uninitialized; stock's dma_* helpers would write outside the 8MB RDRAM or DMEM. Skip those.
static inline void btx_dma_rdram_to_dmem(uint8_t* rdram, uint32_t dmem_addr, uint32_t dram_addr, uint32_t rd_len) {
    rd_len += 1; // Read length is inclusive
    dram_addr &= 0xFFFFF8;
    if (dram_addr + rd_len > 0x800000 || dmem_addr + rd_len > 0x1000) {
        return;
    }
    for (uint32_t i = 0; i < rd_len; i++) {
        RSP_MEM_B(i, dmem_addr) = MEM_B(0, (int64_t)(int32_t)(dram_addr + i + 0x80000000));
    }
}

static inline void btx_dma_dmem_to_rdram(uint8_t* rdram, uint32_t dmem_addr, uint32_t dram_addr, uint32_t wr_len) {
    wr_len += 1; // Write length is inclusive
    dram_addr &= 0xFFFFF8;
    if (dram_addr + wr_len > 0x800000 || dmem_addr + wr_len > 0x1000) {
        return;
    }
    for (uint32_t i = 0; i < wr_len; i++) {
        MEM_B(0, (int64_t)(int32_t)(dram_addr + i + 0x80000000)) = RSP_MEM_B(i, dmem_addr);
    }
}

#undef DO_DMA_READ
#undef DO_DMA_WRITE
#define DO_DMA_READ(rd_len) btx_dma_rdram_to_dmem(rdram, dma_mem_address, dma_dram_address, (rd_len))
#define DO_DMA_WRITE(wr_len) btx_dma_dmem_to_rdram(rdram, dma_mem_address, dma_dram_address, (wr_len))

#endif

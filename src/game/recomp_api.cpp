#include <cmath>

#include "recomp.h"
#include "librecomp/overlays.hpp"
#include "librecomp/addresses.hpp"
#include "battletanx_config.h"
#include "recompinput/recompinput.h"
#include "recompui/recompui.h"
#include "recompui/renderer.h"
#include "librecomp/helpers.hpp"
#include "../patches/input.h"
#include "../patches/graphics.h"
#include "../patches/sound.h"
#include "ultramodern/ultramodern.hpp"
#include "ultramodern/config.hpp"
#include "../lib/N64ModernRuntime/thirdparty/xxHash/xxh3.h"

extern "C" void recomp_update_inputs(uint8_t* rdram, recomp_context* ctx) {
    recompinput::poll_inputs();
}

extern "C" void recomp_puts(uint8_t* rdram, recomp_context* ctx) {
    PTR(char) cur_str = _arg<0, PTR(char)>(rdram, ctx);
    u32 length = _arg<1, u32>(rdram, ctx);

    for (u32 i = 0; i < length; i++) {
        fputc(MEM_B(i, (gpr)cur_str), stdout);
    }
}

extern "C" void recomp_exit(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::quit();
}

extern "C" void recomp_error(uint8_t* rdram, recomp_context* ctx) {
    std::string str{};
    PTR(u8) str_ptr = _arg<0, PTR(u8)>(rdram, ctx);

    for (size_t i = 0; MEM_B(str_ptr, i) != '\x00'; i++) {
        str += (char)MEM_B(str_ptr, i);
    }

    recompui::message_box(str.c_str());
    assert(false);
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

extern "C" void recomp_get_gyro_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    // TODO: use controller number
    recompinput::get_gyro_deltas(0, x_out, y_out);
}

extern "C" void recomp_get_mouse_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    recompinput::get_mouse_deltas(x_out, y_out);
}

extern "C" void recomp_powf(uint8_t* rdram, recomp_context* ctx) {
    float a = _arg<0, float>(rdram, ctx);
    float b = ctx->f14.fl; //_arg<1, float>(rdram, ctx);

    _return(ctx, std::pow(a, b));
}

extern "C" void recomp_get_target_framerate(uint8_t* rdram, recomp_context* ctx) {
    int frame_divisor = _arg<0, u32>(rdram, ctx);

    _return(ctx, ultramodern::get_target_framerate(60 / frame_divisor));
}

extern "C" void recomp_get_window_resolution(uint8_t* rdram, recomp_context* ctx) {
    int width, height;
    recompui::get_window_size(width, height);

    gpr width_out = _arg<0, PTR(u32)>(rdram, ctx);
    gpr height_out = _arg<1, PTR(u32)>(rdram, ctx);

    MEM_W(0, width_out) = (u32)width;
    MEM_W(0, height_out) = (u32)height;
}

extern "C" void recomp_get_target_aspect_ratio(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::renderer::GraphicsConfig graphics_config = ultramodern::renderer::get_graphics_config();
    float original = _arg<0, float>(rdram, ctx);
    int width, height;
    recompui::get_window_size(width, height);

    switch (graphics_config.ar_option) {
        case ultramodern::renderer::AspectRatio::Original:
        default:
            _return(ctx, original);
            return;
        case ultramodern::renderer::AspectRatio::Expand:
            _return(ctx, std::max(static_cast<float>(width) / height, original));
            return;
    }
}

extern "C" void recomp_time_us(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<u32>(std::chrono::duration_cast<std::chrono::microseconds>(ultramodern::time_since_start()).count()));
}

extern "C" void recomp_load_overlays(uint8_t * rdram, recomp_context * ctx) {
    u32 rom = _arg<0, u32>(rdram, ctx);
    PTR(void) ram = _arg<1, PTR(void)>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    load_overlays(rom, ram, size);
}

extern "C" void recomp_high_precision_fb_enabled(uint8_t * rdram, recomp_context * ctx) {
    _return(ctx, static_cast<s32>(recompui::renderer::RT64HighPrecisionFBEnabled()));
}

extern "C" void recomp_get_resolution_scale(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, ultramodern::get_resolution_scale());
}

extern "C" void recomp_get_right_analog_inputs(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    // Don't return right analog inputs while game input is disabled.
    if (recompinput::game_input_disabled()) {
        *x_out = 0.0f;
        *y_out = 0.0f;
        return;
    }

    // TODO: Use controller number.
    recompinput::get_right_analog(0, x_out, y_out);

    // Radial deadzone of 0.05, as in Zelda64Recomp, so stick noise doesn't cause drift. Rescale
    // so full deflection still reaches 1.0 once the deadzone is removed.
    {
        constexpr float radial_deadzone = 0.05f;
        float x = *x_out, y = *y_out;
        float mag = sqrtf(x * x + y * y);
        if (mag < radial_deadzone) {
            *x_out = 0.0f;
            *y_out = 0.0f;
        } else {
            float xn = x / mag, yn = y / mag;
            float scale = (mag - radial_deadzone) / (1.0f - radial_deadzone);
            *x_out = xn * scale;
            *y_out = yn * scale;
        }
    }
}

extern "C" void recomp_get_camera_inputs(uint8_t* rdram, recomp_context* ctx) {
    // Alias for Zelda parity: same deadzone logic, used by dual-analog camera patches.
    // Exposed for mods/patches that expect recomp_get_camera_inputs (Zelda 0.05 deadzone).
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);
    if (recompinput::game_input_disabled()) {
        *x_out = 0.0f;
        *y_out = 0.0f;
        return;
    }
    recompinput::get_right_analog(0, x_out, y_out);
    constexpr float radial_deadzone = 0.05f;
    float x = *x_out, y = *y_out;
    float mag = sqrtf(x * x + y * y);
    if (mag < radial_deadzone) {
        *x_out = 0.0f;
        *y_out = 0.0f;
    } else {
        float xn = x / mag, yn = y / mag;
        float scale = (mag - radial_deadzone) / (1.0f - radial_deadzone);
        *x_out = xn * scale;
        *y_out = yn * scale;
    }
}

extern "C" void recomp_set_right_analog_suppressed(uint8_t* rdram, recomp_context* ctx) {
    s32 suppressed = _arg<0, s32>(rdram, ctx);

    recompinput::set_right_analog_suppressed(suppressed);
}

constexpr uint32_t k1_to_phys(uint32_t addr) {
    return addr & 0x1FFFFFFF;
}

extern "C" void osPiReadIo_recomp(RDRAM_ARG recomp_context * ctx) {
    uint32_t devAddr = recomp::rom_base | ctx->r4;
    gpr dramAddr = ctx->r5;
    uint32_t physical_addr = k1_to_phys(devAddr);

    if (physical_addr > recomp::rom_base) {
        // cart rom
        recomp::do_rom_pio(PASS_RDRAM dramAddr, physical_addr);
    } else {
        // sram
        assert(false && "SRAM ReadIo unimplemented");
    }

    ctx->r2 = 0;
}

extern "C" void osPiWriteIo_recomp(RDRAM_ARG recomp_context * ctx) {
    // BattleTanx's "osPiWriteIo" at 0x80119100 is actually osPiRawStartDma
    // (per the n64sym SDK scan): the PIMGR thread (func_80118AF0) dispatches PI
    // requests through a function-pointer table ([[pi_manager]+0x14]) with the
    // raw-DMA signature. MIPS o32 args (as the game issues them):
    //   a0 = direction (0 = OS_READ, cart -> RDRAM)
    //   a1 = devAddr   (0xB0xxxxxx, cart physical address)
    //   a2 = dramAddr  (0x80xxxxxx, RDRAM destination)
    //   a3 = size
    // The game polls osPiGetStatus (runtime returns 0 = idle) for completion, so a
    // synchronous copy is sufficient (no completion message needed).
    uint32_t direction = ctx->r4;
    uint32_t devAddr = ctx->r5;
    gpr dramAddr = ctx->r6;
    uint32_t size = ctx->r7;
    uint32_t physical_addr = k1_to_phys(devAddr);

    if (direction == 0 && physical_addr >= recomp::rom_base) {
        // Synchronous cart->RDRAM read. The ROM stores big-endian bytes; the recomp's
        // RDRAM holds the game's 32-bit values in host (little-endian) form (MEM_W has
        // no swap; MEM_H/MEM_B swap via address XOR), so each 4-byte group must be
        // byte-reversed to match what the game's `lw`/RT64 would read. Tolerates
        // misaligned addresses (the game issues odd devAddr values) by copying the
        // unaligned remainder byte-for-byte. The size is clamped to the ROM bounds:
        // some callers pass misaligned/overlapping args (the game also calls this
        // primitive with a second, differently-packed layout), and an unclamped copy
        // would read past the end of the ROM and SIGSEGV.
        std::span<const uint8_t> rom = recomp::get_rom();
        uint32_t rom_off = physical_addr - recomp::rom_base;
        if (rom_off < rom.size()) {
            size_t copy_size = std::min<size_t>(size, rom.size() - rom_off);
            const uint8_t* rom_addr = rom.data() + rom_off;
            uint32_t rdr = ((uint32_t)dramAddr) & 0x1FFFFFFF;
            uint32_t i = 0;
            if (((physical_addr | rdr) & 3) == 0) {
                for (; i + 4 <= copy_size; i += 4) {
                    uint32_t w = ((uint32_t)rom_addr[i] << 24) | ((uint32_t)rom_addr[i+1] << 16) |
                                 ((uint32_t)rom_addr[i+2] << 8) | (uint32_t)rom_addr[i+3];
                    *(uint32_t*)(rdram + rdr + i) = w;
                }
            }
            for (; i < copy_size; i++) {
                rdram[rdr + i] = rom_addr[i];
            }
        }
    }
    ctx->r2 = 0;
}

// u32 rom_addr, void *ram_addr, u32 size
extern "C" void recomp_load_overlays_by_rom(uint8_t* rdram, recomp_context* ctx) {
    u32 rom_addr = _arg<0, u32>(rdram, ctx);
    PTR(void) ram_addr = _arg<1, PTR(void)>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    load_overlays(rom_addr, ram_addr, size);
}

extern "C" void recomp_abort(uint8_t* rdram, recomp_context* ctx) {
    std::string msg = _arg_string<0>(rdram, ctx);
    recompui::message_box(msg.c_str());
    assert(false);
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

extern "C" void recomp_xxh3(uint8_t* rdram, recomp_context* ctx) {
    PTR(void) data = _arg<0, PTR(void)>(rdram, ctx);
    u32 size = _arg<1, u32>(rdram, ctx);
    XXH3_state_t xxh3;
    XXH3_64bits_reset(&xxh3);

    // Hash 1 byte at a time to account for byteswapping.
    for (size_t i = 0; i < size; i++) {
        XXH3_64bits_update(&xxh3, TO_PTR(u8, data + i), 1);
    }

    uint64_t ret = XXH3_64bits_digest(&xxh3);
    
    ctx->r2 = (int32_t)(ret >> 32);
    ctx->r3 = (int32_t)(ret >> 0);
}

// Level select: returns 1 to enable the game's built-in level select screen,
// 0 to skip it and start on the first campaign level.
extern "C" void recomp_get_level_select(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, battletanx::get_level_select_enabled() ? 1 : 0);
}

// Cheat toggles, indexed to match the game's cheat code table order
// (skipping the unenterable "TOD CHEAT" dummy at index 3).
extern "C" void recomp_get_cheat_enabled(uint8_t* rdram, recomp_context* ctx) {
    s32 index = _arg<0, s32>(rdram, ctx);
    _return<s32>(ctx, battletanx::get_cheat_enabled(index));
}

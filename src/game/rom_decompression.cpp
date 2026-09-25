#include <cstdint>

#include "recomp.h"
#include "battletanx_game.h"

void battletanx::btx_on_init(uint8_t* rdram, recomp_context* ctx) {
    // BattleTanx boots via the CIC 6102 IPL3 (see tanx-progress.md).
    MEM_W(0, (int32_t)0x80000310) = 6102;
}

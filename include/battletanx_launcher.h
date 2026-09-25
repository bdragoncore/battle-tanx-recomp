#ifndef __BATTLE_TANX_LAUNCHER_H__
#define __BATTLE_TANX_LAUNCHER_H__

#include "recompui/recompui.h"

namespace battletanx {
    void launcher_animation_setup(recompui::LauncherMenu *menu);
    void launcher_animation_update(recompui::LauncherMenu *menu);

    constexpr float launcher_options_right_position_start = 96.0f;
    constexpr float launcher_options_right_position_end = 96.0f + 24.0f;
}

#endif

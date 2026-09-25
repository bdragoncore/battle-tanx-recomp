#ifndef __BATTLE_TANX_CONFIG_H__
#define __BATTLE_TANX_CONFIG_H__

#include <filesystem>
#include <string>
#include <string_view>

#include "json/json.hpp"

namespace battletanx {
    inline const std::u8string program_id = u8"BattleTanxRecompiled";
    inline const std::string program_name = "BattleTanx: Recompiled";

    namespace configkeys {
        namespace cheats {
            inline const std::string tab_id = "cheats";
            inline const std::string level_select = "level_select";
            inline const std::string unlimited_ammo = "cheat_unlimited_ammo";
            inline const std::string all_weapons = "cheat_all_weapons";
            inline const std::string invulnerable = "cheat_invulnerable";
            inline const std::string invisible = "cheat_invisible";
            inline const std::string hurl_mode = "cheat_hurl_mode";
            inline const std::string storm_ravens = "cheat_storm_ravens";
            inline const std::string run_story = "cheat_run_story";
            inline const std::string trippy = "cheat_trippy";
            inline const std::string unlimited_lives = "cheat_unlimited_lives";
            inline const std::string campaign_gangs = "cheat_campaign_gangs";
            inline const std::string frogs = "cheat_frogs";
            inline const std::string toads = "cheat_toads";
        }
    }

    // TODO: Move loading configs to the runtime once we have a way to allow per-project customization.
    void init_config();

    // Level select: true enables the game's built-in level select screen
    // (the game's normal flow). When false, the game skips the level select
    // screen and starts on the first campaign level.
    bool get_level_select_enabled();

    // Cheat toggles, indexed to match the game's cheat code table order
    // (skipping the unenterable "TOD CHEAT" dummy). Returns true if enabled.
    bool get_cheat_enabled(int index);

    void open_quit_game_prompt();
};

#endif

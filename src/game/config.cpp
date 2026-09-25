#include "battletanx_config.h"
#include "recompui/recompui.h"
#include "recompui/config.h"
#include "recompinput/recompinput.h"
#include "battletanx_support.h"
#include "ultramodern/config.hpp"
#include "librecomp/files.hpp"
#include "librecomp/config.hpp"
#include "util/file.h"
#include <filesystem>
#include <fstream>
#include <iomanip>

#if defined(_WIN32)
#include <Shlobj.h>
#elif defined(__linux__)
#include <unistd.h>
#include <pwd.h>
#elif defined(__APPLE__)
#include "apple/rt64_apple.h"
#endif

static void add_cheats_options(recomp::config::Config &config) {
    config.add_bool_option(
        battletanx::configkeys::cheats::level_select,
        "Level Select",
        "Enables the game's built-in level select screen, letting you pick any level before a match. When disabled, the game skips the level select screen and starts on the first campaign level (New York - Queens).",
        true
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::unlimited_ammo,
        "Unlimited Ammo",
        "Prevents your ammo from being consumed when firing."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::all_weapons,
        "All Weapons",
        "Grants every weapon at match start."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::invulnerable,
        "Invulnerable",
        "Your tank takes no damage."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::invisible,
        "Invisible",
        "Your tank is invisible to enemies."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::hurl_mode,
        "Hurl Mode",
        "Changes your tank's movement into a spinning hurl."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::storm_ravens,
        "Storm Ravens",
        "Unlocks the Storm Raven character in the character select."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::run_story,
        "Run Story",
        "Skips the title screen and goes straight to the main menu."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::trippy,
        "Trippy",
        "Enables psychedelic particle effects."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::unlimited_lives,
        "Unlimited Lives",
        "Prevents your lives from being consumed."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::campaign_gangs,
        "Campaign Gangs",
        "Changes the level load flow to campaign gangs mode."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::frogs,
        "Frogs",
        "Changes the HUD display to frogs."
    );
    config.add_bool_option(
        battletanx::configkeys::cheats::toads,
        "Toads",
        "Changes your tank's appearance to a toad."
    );
}

bool get_cheats_config_bool_value(const std::string& option_id) {
    return std::get<bool>(recompui::config::get_config(battletanx::configkeys::cheats::tab_id).get_option_value(option_id));
}

bool battletanx::get_level_select_enabled() {
    return get_cheats_config_bool_value(battletanx::configkeys::cheats::level_select);
}

bool battletanx::get_cheat_enabled(int index) {
    switch (index) {
        case 0: return get_cheats_config_bool_value(battletanx::configkeys::cheats::unlimited_ammo);
        case 1: return get_cheats_config_bool_value(battletanx::configkeys::cheats::all_weapons);
        case 2: return get_cheats_config_bool_value(battletanx::configkeys::cheats::invulnerable);
        case 3: return get_cheats_config_bool_value(battletanx::configkeys::cheats::invisible);
        case 4: return get_cheats_config_bool_value(battletanx::configkeys::cheats::hurl_mode);
        case 5: return get_cheats_config_bool_value(battletanx::configkeys::cheats::storm_ravens);
        case 6: return get_cheats_config_bool_value(battletanx::configkeys::cheats::run_story);
        case 7: return get_cheats_config_bool_value(battletanx::configkeys::cheats::trippy);
        case 8: return get_cheats_config_bool_value(battletanx::configkeys::cheats::unlimited_lives);
        case 9: return get_cheats_config_bool_value(battletanx::configkeys::cheats::campaign_gangs);
        case 10: return get_cheats_config_bool_value(battletanx::configkeys::cheats::frogs);
        case 11: return get_cheats_config_bool_value(battletanx::configkeys::cheats::toads);
        default: return false;
    }
}

static void set_control_defaults() {
    using namespace recompinput;

    // Left shoulder -> C Down | Change tank / rail controls
    set_default_mapping_for_controller(
        GameInput::C_DOWN,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, true),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        }
    );

    // Right shoulder -> C Up | Change tank / rail controls
    set_default_mapping_for_controller(
        GameInput::C_UP,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, false),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
        }
    );

    // North button -> C Left | Change tank / rail controls
    set_default_mapping_for_controller(
        GameInput::C_LEFT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, false),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_NORTH)
        }
    );

    // East button -> C Right | Change tank / rail controls
    set_default_mapping_for_controller(
        GameInput::C_RIGHT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, true),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_EAST)
        }
    );

    // R3 -> L | Change viewpoint
    set_default_mapping_for_controller(GameInput::L, { InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK) });
}

static void set_control_descriptions() {
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_POS, "Used to steer your tank and aim the turret.");
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_NEG, "Used to steer your tank and aim the turret.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_NEG, "Used to steer your tank and aim the turret.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_POS, "Used to steer your tank and aim the turret.");
    recompinput::set_game_input_description(recompinput::GameInput::A, "Fires your secondary weapon. Also used to cycle through secondary weapons and confirm selections in menus.");
    recompinput::set_game_input_description(recompinput::GameInput::B, "Used to go back in menus.");
    recompinput::set_game_input_description(recompinput::GameInput::Z, "Fires your main weapon.");
    recompinput::set_game_input_description(recompinput::GameInput::L, "Changes the camera viewpoint.");
    recompinput::set_game_input_description(recompinput::GameInput::R, "Toggles strafe mode.");
    recompinput::set_game_input_description(recompinput::GameInput::START, "Pauses the game. Also used to start the game.");
    recompinput::set_game_input_description(recompinput::GameInput::C_UP, "Changes your active tank. Also used for rail movement in bonus levels.");
    recompinput::set_game_input_description(recompinput::GameInput::C_DOWN, "Changes your active tank. Also used for rail movement in bonus levels.");
    recompinput::set_game_input_description(recompinput::GameInput::C_LEFT, "Changes your active tank. Also used for rail movement in bonus levels.");
    recompinput::set_game_input_description(recompinput::GameInput::C_RIGHT, "Changes your active tank. Also used for rail movement in bonus levels.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_UP, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_DOWN, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_LEFT, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_RIGHT, "Unused. Mods may use it for additional features.");
}

void battletanx::init_config() {
    std::filesystem::path recomp_dir = recompui::file::get_app_folder_path();

    if (!recomp_dir.empty()) {
        std::filesystem::create_directories(recomp_dir);
    }

    recompui::config::GeneralTabOptions general_options{};
    general_options.has_rumble_strength = true;
    general_options.has_gyro_sensitivity = false;
    general_options.has_mouse_sensitivity = false;

    recompui::config::create_general_tab(general_options);

    recompui::config::create_graphics_tab();

    set_control_defaults();
    set_control_descriptions();
    recompui::config::create_controls_tab();

    recompui::config::create_sound_tab();

    auto &cheats_config = recompui::config::create_config_tab("Cheats", battletanx::configkeys::cheats::tab_id, false);
    add_cheats_options(cheats_config);

    recompui::config::create_mods_tab();

    recompui::config::finalize();

}

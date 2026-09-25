#include "battletanx_launcher.h"
#include "elements/ui_image.h"
#include "util/file.h"
#include <atomic>
#include <fstream>

struct Keyframe2D {
    float seconds;
    float x;
    float y;
};

enum class InterpolationMethod {
    Linear,
    Smootherstep
};

struct AnimationData {
    uint32_t keyframe_index = 0;
    uint32_t loop_keyframe_index = UINT32_MAX;
    float seconds = 0.0f;
    InterpolationMethod interpolation_method = InterpolationMethod::Linear;
};

struct LauncherContext {
    recompui::Element *wrapper;
    float wrapper_phase = -1.0f;
    std::chrono::steady_clock::time_point last_update_time;
    float seconds = 0.0f;
    bool started = false;
    bool options_enabled = false;
    bool animation_skipped = false;
    std::atomic<bool> skip_animation_next_update = false;
} launcher_context;

float interpolate_value(float a, float b, float t, InterpolationMethod method) {
    switch (method) {
    case InterpolationMethod::Smootherstep:
        return a + (b - a) * (t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f));
    case InterpolationMethod::Linear:
    default:
        return a + (b - a) * t;
    }
}

bool check_skip_input(SDL_Event* event) {
    switch (event->type) {
    case SDL_KEYDOWN:
        return event->key.keysym.scancode == SDL_SCANCODE_ESCAPE ||
            event->key.keysym.scancode == SDL_SCANCODE_SPACE ||
            (event->key.keysym.scancode == SDL_SCANCODE_RETURN && (event->key.keysym.mod & (KMOD_LALT | KMOD_RALT)) == KMOD_NONE);
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_MOUSEBUTTONDOWN:
        return true;
    default:
        return false;
    }
}

int launcher_event_watch(void* userdata, SDL_Event* event) {
    if (!launcher_context.animation_skipped && check_skip_input(event)) {
        launcher_context.animation_skipped = true;
        launcher_context.skip_animation_next_update = true;
        return 0;
    }
    else {
        return 1;
    }
}

const float options_slide_start = 0.0f;
const float options_slide_length = 1.5f;
const float options_slide_end = options_slide_start + options_slide_length;

const float animation_skip_time = 10.0f;

void battletanx::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element *background_container = menu->get_background_container();

    // Load the box art as the launcher background.
    std::vector<char> background_bytes;
    {
        std::ifstream file(recompui::file::get_asset_path("Background.png"), std::ios::binary);
        if (file) {
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            background_bytes.resize(size);
            file.read(background_bytes.data(), size);
        }
    }
    if (!background_bytes.empty()) {
        recompui::queue_image_from_bytes_file("?/background", background_bytes);
    }

    recompui::Image *background = context.create_element<recompui::Image>(background_container, "?/background");
    background->set_position(recompui::Position::Absolute);
    background->set_top(50.0f, recompui::Unit::Percent);
    background->set_left(0);
    background->set_height_auto();
    background->set_width(100.0f, recompui::Unit::Percent);
    background->set_translate_2D(0.0f, -50.0f, recompui::Unit::Percent);
    background_container->set_overflow(recompui::Overflow::Hidden);

    // Disable and hide the options until the slide-in animation completes.
    for (auto option : menu->get_game_options_menu()->get_options()) {
        option->set_enabled(false);
        option->set_opacity(0.0f);
    }

    // Install an event watch to skip the launcher animation if a keyboard, mouse or controller input is detected.
    SDL_AddEventWatch(&launcher_event_watch, nullptr);
}

void battletanx::launcher_animation_update(recompui::LauncherMenu *menu) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float delta_time = launcher_context.started ? std::chrono::duration_cast<std::chrono::milliseconds>(now - launcher_context.last_update_time).count() / 1000.0f : 0.0f;
    if (launcher_context.skip_animation_next_update) {
        delta_time = std::max(animation_skip_time - launcher_context.seconds, 0.0f);
        launcher_context.skip_animation_next_update = false;
    }

    launcher_context.seconds += delta_time;
    launcher_context.last_update_time = now;
    launcher_context.started = true;

    float phase = std::clamp((launcher_context.seconds - options_slide_start) / (options_slide_end - options_slide_start), 0.0f, 1.0f);
    if (phase != launcher_context.wrapper_phase) {
        float game_option_menu_right = interpolate_value(launcher_options_right_position_start, launcher_options_right_position_end, phase, InterpolationMethod::Smootherstep);
        menu->get_game_options_menu()->set_right(game_option_menu_right);

        float game_option_menu_opacity = interpolate_value(0, 1.0f, phase, InterpolationMethod::Smootherstep);
        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_opacity(game_option_menu_opacity);
        }

        launcher_context.wrapper_phase = phase;
    }

    if (!launcher_context.options_enabled && launcher_context.seconds >= options_slide_end) {
        SDL_DelEventWatch(&launcher_event_watch, nullptr);

        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_enabled(true);
            option->set_opacity(1.0f);
        }

        launcher_context.options_enabled = true;
    }
}
#include <cstdio>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <array>
#include <filesystem>
#include <numeric>
#include <stdexcept>
#include <cinttypes>
#include <atomic>
#include <cmath>

#include "nfd.h"

#include "ultramodern/ultra64.h"
#include "ultramodern/ultramodern.hpp"
#include "ultramodern/config.hpp"
#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
// Undefine x11 macros that get included by SDL_syswm.h.
#undef None
#undef Status
#undef LockMask
#undef ControlMask
#undef Success
#undef Always
#endif

#include "recompui/recompui.h"

#include "recompui/program_config.h"
#include "recompui/renderer.h"
#include "recompui/config.h"
#include "util/file.h"
#include "recompinput/input_events.h"
#include "recompinput/recompinput.h"
#include "recompinput/profiles.h"
#include "battletanx_config.h"
#include "battletanx_support.h"
#include "battletanx_game.h"
#include "battletanx_launcher.h"
#include "recomp_data.h"
#include "ovl_patches.hpp"
#include "theme.h"
#include "librecomp/game.hpp"
#include "librecomp/mods.hpp"
#include "librecomp/helpers.hpp"

#include "../../patches/graphics.h"
#include "../../patches/input.h"
#include "../../patches/sound.h"
#include "../../patches/misc_funcs.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#include "SDL_syswm.h"
#define APP_ICON 1
#endif

#include "../../lib/rt64/src/contrib/stb/stb_image.h"

const std::string version_string = "1.0.0";

template<typename... Ts>
void exit_error(const char* str, Ts ...args) {
    // TODO pop up an error
    ((void)fprintf(stderr, str, args), ...);
    assert(false);
        
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

ultramodern::gfx_callbacks_t::gfx_data_t create_gfx() {
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) > 0) {
        exit_error("Failed to initialize SDL2: %s\n", SDL_GetError());
    }

    fprintf(stdout, "SDL Video Driver: %s\n", SDL_GetCurrentVideoDriver());

    return {};
}

#ifdef BTX_FORKED_RUNTIME
// The forked runtime implements a Controller Pak (ultramodern/src/pfs.cpp).
static constexpr ultramodern::input::Pak kBtxCardPak = ultramodern::input::Pak::ControllerPak;
#else
static constexpr ultramodern::input::Pak kBtxCardPak = ultramodern::input::Pak::RumblePak;
#endif

ultramodern::input::connected_device_info_t get_connected_device_info(int controller_num) {
    // Report exactly ONE connected controller (channel 0) in single-player mode: the
    // game's controller scan counts responding channels and branches its boot flow on
    // the count (e.g. 4 controllers skips the single-player title screens). Multiplayer
    // reports only the assigned players' channels.
    if (controller_num == 0 && (recompinput::players::is_single_player_mode() || recompinput::players::get_player_is_assigned(0))) {
        return ultramodern::input::connected_device_info_t{
            .connected_device = ultramodern::input::Device::Controller,
            // Any pak other than None sets the controller status's card-inserted bit, which the
            // game checks before calling osPfsInitPak. The stock runtime's Pak enum has no
            // ControllerPak; src/game/controller_pak.cpp answers the osPfs calls.
            .connected_pak = kBtxCardPak,
        };
    }

    if (!recompinput::players::is_single_player_mode() && recompinput::players::get_player_is_assigned(controller_num)) {
        return ultramodern::input::connected_device_info_t{
            .connected_device = ultramodern::input::Device::Controller,
            // Any pak other than None sets the controller status's card-inserted bit, which the
            // game checks before calling osPfsInitPak. The stock runtime's Pak enum has no
            // ControllerPak; src/game/controller_pak.cpp answers the osPfs calls.
            .connected_pak = kBtxCardPak,
        };
    }

    return ultramodern::input::connected_device_info_t{
        .connected_device = ultramodern::input::Device::None,
        .connected_pak = ultramodern::input::Pak::None,
    };
}

#include "icon_bytes.h"

#if defined(__gnu_linux__)
bool SetImageAsIcon(const char* filename, SDL_Window* window)
{
    // Read data
    int width, height, bytesPerPixel;
    void* data = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(icon_bytes), sizeof(icon_bytes), &width, &height, &bytesPerPixel, 4);

    // Calculate pitch
    int pitch;
    pitch = width * 4;
    pitch = (pitch + 3) & ~3;

    // Setup relevance bitmask
    int Rmask, Gmask, Bmask, Amask;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = 0xFF000000;
#else
    Rmask = 0xFF000000;
    Gmask = 0x00FF0000;
    Bmask = 0x0000FF00;
    Amask = 0x000000FF;
#endif

    SDL_Surface* surface = nullptr;
    if (data != nullptr) {
        surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, pitch, Rmask, Gmask,
                            Bmask, Amask);
    }

    if (surface == nullptr) {   
        if (data != nullptr) {
            stbi_image_free(data);
        }
        return false;
	} else {
        SDL_SetWindowIcon(window,surface);
        SDL_FreeSurface(surface);
        stbi_image_free(data);
        return true;
    }
}
#endif

SDL_Window* window;

ultramodern::renderer::WindowHandle create_window(ultramodern::gfx_callbacks_t::gfx_data_t) {
    uint32_t flags = SDL_WINDOW_RESIZABLE;

#if defined(__APPLE__)
    flags |= SDL_WINDOW_METAL;
#elif defined(RT64_SDL_WINDOW_VULKAN)
    flags |= SDL_WINDOW_VULKAN;
#endif

    window = SDL_CreateWindow(battletanx::program_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,  flags);

    if (window == nullptr) {
        exit_error("Failed to create window: %s\n", SDL_GetError());
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);

#if defined(_WIN32)
    HICON new_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
    SendMessage(wmInfo.info.win.window, WM_SETICON, ICON_SMALL2, (LPARAM)(new_icon));
#elif defined(__linux__)
    SetImageAsIcon("icons/app.png", window);

#endif

#if defined(_WIN32)
    return ultramodern::renderer::WindowHandle{ wmInfo.info.win.window, GetCurrentThreadId() };
#elif defined(__linux__) || defined(__ANDROID__)
    return ultramodern::renderer::WindowHandle{ window };
#elif defined(__APPLE__)
    SDL_MetalView view = SDL_Metal_CreateView(window);
    return ultramodern::renderer::WindowHandle{ wmInfo.info.cocoa.window,  SDL_Metal_GetLayer(view) };
#else
    static_assert(false && "Unimplemented");
#endif
}

void update_gfx(void*) {
    recompinput::handle_events();
}

static SDL_AudioDeviceID audio_device = 0;

// Samples per channel per second.
static uint32_t sample_rate = 48000;
static uint32_t output_sample_rate = 48000;
// Channel count.
constexpr uint32_t input_channels = 2;
static uint32_t output_channels = 2;

// Terminology: a frame is a collection of samples for each channel. e.g. 2 input samples is one input frame. This is unrelated to graphical frames.

// ===== Clock-domain bridge (from PokemonStadiumRecomp's recomp_audio_drc.h) =====
// One persistent band-limited resampler + fill controller, pulled by a real SDL
// audio callback. Replaces both crackle sources in the legacy push path: the
// per-chunk SDL_AudioCVT (boundary discontinuities) and the skip_factor
// sample-decimation valve (hard sample drops when the queue grew). The bridge
// resamples sample_rate->output_sample_rate continuously and never drops a
// sample; the game's osAiGetLength feedback reads the bridge fill instead of
// the raw SDL queue.
#define RECOMP_AUDIO_DRC_IMPL
#include "recomp_audio_drc.h"

static rab_bridge g_bridge;
static int g_bridge_ready = 0;
static SDL_mutex* g_audio_mtx = nullptr;
static uint32_t g_bridge_src = 0;

static double audio_perf_now_ms() {
    return (double)SDL_GetPerformanceCounter() * 1000.0 / (double)SDL_GetPerformanceFrequency();
}

// Timestamp of the most recent bridge pull, for interpolating the DAC's
// continuous drain in get_frames_remaining.
static std::atomic<double> g_last_pull_ms{0.0};

// Build (or rebuild on rate change) the bridge for the current sample_rate.
// N64 is a CLOSED-LOOP audio producer: the game sizes every buffer from
// osAiGetLength feedback, so IT is the rate controller. The DRC's ratio
// warping fought that loop and lost (PokemonStadiumRecomp measured it), so
// max_correction = 0 (exact resample) and the fill equilibrium is set by
// under-reporting in get_frames_remaining.
static void bridge_reinit_locked() {
    if (g_bridge_ready) { rab_free(&g_bridge); g_bridge_ready = 0; }
    rab_config rc; rab_config_defaults(&rc);
    rc.channels = output_channels;
    rc.source_rate = (double)sample_rate;
    rc.host_rate = (double)output_sample_rate;
    rc.target_ms = 24.0;
    rc.ring_ms = 300.0;
    rc.max_correction = 0.0;
    g_bridge_ready = (rab_init(&g_bridge, &rc) == 0);
    g_bridge_src = sample_rate;
}

// Audio thread: pull stereo frames from the bridge (int16) and convert to the
// device's F32 format. Never stalls on producer jitter.
static void audio_cb(void* /*ud*/, Uint8* stream, int len) {
    int frames = len / (int)(output_channels * sizeof(float));
    if (!g_bridge_ready) { SDL_memset(stream, 0, (size_t)len); return; }
    static std::vector<int16_t> tmp;
    if ((int)tmp.size() < frames * output_channels) tmp.resize((size_t)frames * output_channels);
    SDL_LockMutex(g_audio_mtx);
    rab_pull(&g_bridge, tmp.data(), frames);
    SDL_UnlockMutex(g_audio_mtx);
    g_last_pull_ms.store(audio_perf_now_ms(), std::memory_order_relaxed);
    float* out = reinterpret_cast<float*>(stream);
    for (int i = 0; i < frames * output_channels; ++i)
        out[i] = (float)tmp[i] * (1.0f / 32768.0f);
}

void queue_samples(int16_t* audio_data, size_t sample_count) {
    if (audio_device == 0) {
        return;
    }
    size_t frames = sample_count / input_channels;
    if (frames == 0) return;
    // De-swap the channels (libultra interleaves R,L per word) and apply the
    // legacy 0.5 headroom + main volume, then push int16 straight into the
    // bridge. The bridge resamples to the device rate continuously (no
    // SDL_AudioCVT) and the audio callback drains it (no decimation valve).
    static std::vector<int16_t> push_buf;
    if (push_buf.size() < frames * output_channels) push_buf.resize(frames * output_channels);
    float cur_main_volume = static_cast<float>(recompui::config::sound::get_main_volume()) / 100.0f;
    const float scale = 0.5f * cur_main_volume;
    for (size_t f = 0; f < frames; ++f) {
        int l = (int)lrintf((float)audio_data[f * input_channels + 1] * scale);
        int r = (int)lrintf((float)audio_data[f * input_channels + 0] * scale);
        if (l > 32767) l = 32767; if (l < -32768) l = -32768;
        if (r > 32767) r = 32767; if (r < -32768) r = -32768;
        push_buf[f * output_channels + 0] = (int16_t)l;
        push_buf[f * output_channels + 1] = (int16_t)r;
    }
    if (g_bridge_ready) {
        SDL_LockMutex(g_audio_mtx);
        if (g_bridge_src != sample_rate) bridge_reinit_locked();
        rab_push(&g_bridge, push_buf.data(), (int)frames);
        SDL_UnlockMutex(g_audio_mtx);
    }
}

size_t get_frames_remaining() {
    if (audio_device == 0) {
        return 0;
    }
    // Bridge path: the game's AI_LEN pacing must read the BRIDGE fill (the real
    // buffer now), not the SDL queue (which the callback keeps near-empty).
    // The lead we subtract here SETS the steady-state ring depth: the game's
    // feedback loop drives the REPORTED remaining to its own small target, so
    // real fill = game target + lead.
    if (g_bridge_ready) {
        SDL_LockMutex(g_audio_mtx);
        double fill_ms = rab_fill_ms(&g_bridge);
        SDL_UnlockMutex(g_audio_mtx);
        constexpr double lead_ms = 50.0;
        double frames = fill_ms * 0.001 * (double)sample_rate;
        // Interpolate the drain since the last pull so the game's pacing loop
        // sees the hardware-shaped sawtooth instead of ~20ms stair-steps.
        double anchor = g_last_pull_ms.load(std::memory_order_relaxed);
        if (anchor > 0.0) {
            double elapsed_ms = audio_perf_now_ms() - anchor;
            if (elapsed_ms < 0.0) elapsed_ms = 0.0;
            if (elapsed_ms > 50.0) elapsed_ms = 50.0;
            frames -= elapsed_ms * 0.001 * (double)sample_rate;
            if (frames < 0.0) frames = 0.0;
        }
        double lag = lead_ms * 0.001 * (double)sample_rate;
        double rem = frames > lag ? frames - lag : 0.0;
        return (size_t)rem;
    }
    // Bridge not ready: report empty so the game keeps producing audio.
    return 0;
}

void set_frequency(uint32_t freq) {
    sample_rate = freq;
    if (audio_device != 0 && g_audio_mtx) {
        SDL_LockMutex(g_audio_mtx);
        bridge_reinit_locked();
        SDL_UnlockMutex(g_audio_mtx);
    }
}

bool reset_audio(uint32_t output_freq) {
    SDL_AudioSpec spec_desired{
        .freq = (int)output_freq,
        .format = AUDIO_F32,
        .channels = (Uint8)output_channels,
        .silence = 0, // calculated
        .samples = 0x200, // 512 frames: above the device period, below the bridge fill
        .padding = 0, // unused
        .size = 0, // calculated
        .callback = audio_cb,
        .userdata = nullptr
    };

    // Read the OBTAINED spec and allow the frequency to change to the
    // device/endpoint native rate. The old call passed obtained=nullptr +
    // flags=0, which forced SDL to accept our hardcoded 48000 and do its OWN
    // internal conversion to the real endpoint rate -- a SECOND resample on top
    // of the bridge's. Target the rate the host actually opened so there is
    // exactly ONE resampler (the bridge).
    SDL_AudioSpec obtained{};
    audio_device = SDL_OpenAudioDevice(nullptr, false, &spec_desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (audio_device == 0) {
        // Headless/CI fallback: container has no pipewire/pulse (/run/udev unreadable).
        // SDL_AUDIODRIVER=dummy is the usual workaround (SDL_AUDIODRIVER=dummy ./BattleTanxRecompiled),
        // but the game should also auto-retry so bare `./BattleTanxRecompiled` doesn't hard-fail.
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            audio_device = SDL_OpenAudioDevice(nullptr, false, &spec_desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
            if (audio_device == 0) {
                fprintf(stderr, "Could not open an audio device: %s\n", SDL_GetError());
            }
        } else {
            fprintf(stderr, "Could not initialise audio: %s\n", SDL_GetError());
        }
    }

    if (audio_device == 0) {
        // No audio device available — continue without audio (silent mode) instead of EXIT_FAILURE.
        // queue_samples/get_frames_remaining now guard audio_device==0 and become no-ops.
        fprintf(stderr, "No audio device available, continuing without sound.\n");
        output_sample_rate = output_freq;
        return true;
    }

    // Target everything at the rate SDL actually gave us, never the hardcoded one.
    output_sample_rate = (obtained.freq > 0) ? (uint32_t)obtained.freq : output_freq;
    if (!g_audio_mtx) g_audio_mtx = SDL_CreateMutex();
    SDL_LockMutex(g_audio_mtx);
    bridge_reinit_locked();
    SDL_UnlockMutex(g_audio_mtx);
    SDL_PauseAudioDevice(audio_device, 0);

    return true;
}

extern RspUcodeFunc battletanx_audio;
extern RspUcodeFunc f3dex;

// src/game/stock_runtime_compat.cpp: task prep the stock runtime doesn't do.
RspUcodeFunc* btx_wrap_rsp_microcode(const OSTask* task, RspUcodeFunc* ucode);
void btx_set_rdram(uint8_t* rdram);

static RspUcodeFunc* select_rsp_microcode(const OSTask* task);

RspUcodeFunc* get_rsp_microcode(const OSTask* task) {
#ifdef BTX_FORKED_RUNTIME
    return select_rsp_microcode(task); // The forked runtime prepares RSP tasks itself.
#else
    return btx_wrap_rsp_microcode(task, select_rsp_microcode(task));
#endif
}

static RspUcodeFunc* select_rsp_microcode(const OSTask* task) {
    switch (task->t.type) {
    case M_AUDTASK:
        return battletanx_audio;

    case M_GFXTASK:
        // BattleTanx's GFX tasks run F3DEX 1.21 (every M_GFXTASK submission uses the ucode at
        // 0x8010E230, which is embedded in the ROM). Only reached when the GFX dispatch calls
        // recomp::rsp::run_task() for the task; RT64's HLE walker is the default path otherwise.
        return f3dex;

    default:
        fprintf(stderr, "Unknown task: %" PRIu32 "\n", task->t.type);
        return nullptr;
    }
}

extern "C" void recomp_entrypoint(uint8_t * rdram, recomp_context * ctx);
// src/game/stock_runtime_compat.cpp: the game's entry without its trailing break.
extern "C" void btx_entrypoint(uint8_t * rdram, recomp_context * ctx);
gpr get_entrypoint_address();

// array of supported GameEntry objects
std::vector<recomp::GameEntry> supported_games = {
    {
        .rom_hash = 0x8E485E76E7448BF3ULL,
        .internal_name = "BattleTanx",
        .display_name = "BattleTanx",
        .game_id = u8"btx.n64.us.1.0",
        .mod_game_id = "btx",
        // BattleTanx uses the standard 4Kbit EEPROM for saves.
        .save_type = recomp::SaveType::Eep4k,
        .thumbnail_bytes = std::span<const char>(icon_bytes),
        .is_enabled = true,
        .decompression_routine = nullptr,
        .has_compressed_code = false,
        .entrypoint_address = get_entrypoint_address(),
        #ifdef BTX_FORKED_RUNTIME
        .entrypoint = recomp_entrypoint,
#else
        .entrypoint = btx_entrypoint,
#endif
        .on_init_callback = battletanx::btx_on_init,
    },
};

// TODO: move somewhere else
namespace battletanx {
    std::string get_game_thread_name(const OSThread* t) {
        std::string name = "[Game] ";

        switch (t->id) {
            case 0:
                switch (t->priority) {
                    case 150:
                        name += "PIMGR";
                        break;

                    case 80:
                        name += "VIMGR";
                        break;

                    default:
                        name += std::to_string(t->id);
                        break;
                }
                break;
            case 1:
                name += "INIT";
                break;
            case 2:
                name += "DEFRAG";
                break;
            case 4:
                name += "AUDIO";
                break;
            case 5:
                name += "RESET";
                break;
            case 6:
                name += "MAIN";
                break;
            case 7:
                name += "CONT";
                break;
            case 8:
                name += "RUMBLE";
                break;
            default:
                name += std::to_string(t->id);
                break;
        }

        return name;
    }
}

#ifdef _WIN32

struct PreloadContext {
    HANDLE handle;
    HANDLE mapping_handle;
    SIZE_T size;
    PVOID view;
};

bool preload_executable(PreloadContext& context) {
    wchar_t module_name[MAX_PATH];
    GetModuleFileNameW(NULL, module_name, MAX_PATH);

    context.handle = CreateFileW(module_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (context.handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to load executable into memory!");
        context = {};
        return false;
    }

    LARGE_INTEGER module_size;
    if (!GetFileSizeEx(context.handle, &module_size)) {
        fprintf(stderr, "Failed to get size of executable!");
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    context.size = module_size.QuadPart;

    context.mapping_handle = CreateFileMappingW(context.handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (context.mapping_handle == nullptr) {
        fprintf(stderr, "Failed to create file mapping of executable!");
        CloseHandle(context.handle);
        context = {};
        return EXIT_FAILURE;
    }

    context.view = MapViewOfFile(context.mapping_handle, FILE_MAP_READ, 0, 0, 0);
    if (context.view == nullptr) {
        fprintf(stderr, "Failed to map view of of executable!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    DWORD pid = GetCurrentProcessId();
    HANDLE process_handle = OpenProcess(PROCESS_SET_QUOTA | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (process_handle == nullptr) {
        fprintf(stderr, "Failed to open own process!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    SIZE_T minimum_set_size, maximum_set_size;
    if (!GetProcessWorkingSetSize(process_handle, &minimum_set_size, &maximum_set_size)) {
        fprintf(stderr, "Failed to get working set size!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    if (!SetProcessWorkingSetSize(process_handle, minimum_set_size + context.size, maximum_set_size + context.size)) {
        fprintf(stderr, "Failed to set working set size!");
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }

    if (VirtualLock(context.view, context.size) == 0) {
        fprintf(stderr, "Failed to lock view of executable! (Error: %08lx)\n", GetLastError());
        CloseHandle(context.mapping_handle);
        CloseHandle(context.handle);
        context = {};
        return false;
    }
    
    return true;
}

void release_preload(PreloadContext& context) {
    VirtualUnlock(context.view, context.size);
    CloseHandle(context.mapping_handle);
    CloseHandle(context.handle);
    context = {};
}

#elif defined(__linux__) || defined(APPLE)

struct PreloadContext {

};

bool preload_executable(PreloadContext& context) {
    // Preloading isn't implemented on Linux and MacOS, but it's also unnecessary there, as the OS already preloads the executable.
    // Therefore, we can just consider the executable to be preloaded.
    return true;
}

void release_preload(PreloadContext& context) {
}

#else

struct PreloadContext {};

bool preload_executable(PreloadContext& context) {
    return false;
}

void release_preload(PreloadContext& context) {
}

#endif

void enable_texture_pack(recomp::mods::ModContext& context, const recomp::mods::ModHandle& mod) {
    recompui::renderer::enable_texture_pack(context, mod);
}

void disable_texture_pack(recomp::mods::ModContext&, const recomp::mods::ModHandle& mod) {
    recompui::renderer::disable_texture_pack(mod);
}

void reorder_texture_pack(recomp::mods::ModContext&) {
    recompui::renderer::trigger_texture_pack_update();
}

void on_launcher_update(recompui::LauncherMenu *menu) {
    battletanx::launcher_animation_update(menu);
}

void on_launcher_init(recompui::LauncherMenu *menu) {
    auto game_options_menu = menu->init_game_options_menu(
        supported_games[0].game_id,
        supported_games[0].mod_game_id,
        supported_games[0].display_name,
        supported_games[0].thumbnail_bytes,
        recompui::GameOptionsMenuLayout::Center
    );

    game_options_menu->add_default_options();
    game_options_menu->set_width(30, recompui::Unit::Percent);

    for (auto option : game_options_menu->get_options()) {
        option->set_justify_content(recompui::JustifyContent::FlexEnd);
        option->set_border_radius(0);

        std::vector<recompui::Style *> hover_focus = {&option->hover_style, &option->focus_style};
        for (auto style : hover_focus) {
            style->set_background_color(recompui::theme::color::Transparent);
        }
    }

    recompui::Element *menu_container = menu->get_menu_container();
    menu_container->set_width(1440);
    menu_container->unset_left();
    menu_container->set_right(50, recompui::Unit::Percent);
    menu_container->set_translate_2D(50.0f, 0.0f, recompui::Unit::Percent);

    game_options_menu->unset_left();
    game_options_menu->set_bottom(50.0f, recompui::Unit::Percent);
    game_options_menu->set_translate_2D(0.0f, 50.0f, recompui::Unit::Percent);
    game_options_menu->set_right(battletanx::launcher_options_right_position_start);

    menu->remove_default_title();

    battletanx::launcher_animation_setup(menu);
}

extern "C" void recomp_get_camera_inputs(uint8_t* rdram, recomp_context* ctx);

#define REGISTER_FUNC(name) recomp::overlays::register_base_export(#name, name)

int main(int argc, char** argv) {
    recomp::Version project_version{};
    if (!recomp::Version::from_string(version_string, project_version)) {
        ultramodern::error_handling::message_box(("Invalid version string: " + version_string).c_str());
        return EXIT_FAILURE;
    }

    // Map this executable into memory and lock it, which should keep it in physical memory. This ensures
    // that there are no stutters from the OS having to load new pages of the executable whenever a new code page is run.
    PreloadContext preload_context;
    bool preloaded = preload_executable(preload_context);

    if (!preloaded) {
        fprintf(stderr, "Failed to preload executable!\n");
    }

    // Initialize random seed for icon easter egg.
    std::srand(std::time(nullptr));

#ifdef _WIN32
    // Set up high resolution timing period.
    timeBeginPeriod(1);

    // Process arguments.
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--show-console") == 0)
        {
            if (GetConsoleWindow() == nullptr)
            {
                AllocConsole();
                freopen("CONIN$", "r", stdin);
                freopen("CONOUT$", "w", stderr);
                freopen("CONOUT$", "w", stdout);
            }

            break;
        }
    }

    // Set up console output to accept UTF-8 on windows
    SetConsoleOutputCP(CP_UTF8);

    // Change to a font that supports Japanese characters
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"NSimSun");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
#endif

#ifdef _WIN32
    // Force wasapi on Windows, as there seems to be some issue with sample queueing with directsound currently.
    SDL_setenv("SDL_AUDIODRIVER", "wasapi", true);
#endif

#if defined(__linux__) && defined(RECOMP_FLATPAK)
    // When using Flatpak, applications tend to launch from the home directory by default.
    // Mods might use the current working directory to store the data, so we switch it to a directory
    // with persistent data storage and write permissions under Flatpak to ensure it works.
    std::error_code ec;
    std::filesystem::current_path("/var/data", ec);
#endif

    // Initialize native file dialogs.
    NFD_Init();

    // Initialize program settings.
    recompui::programconfig::set_program_name(battletanx::program_name);
    recompui::programconfig::set_program_id(battletanx::program_id);
    
    // Initialize SDL audio and set the output frequency. reset_audio() auto-retries with
    // SDL_AUDIODRIVER=dummy and falls back to silent (audio_device==0) so headless/CI
    // without pipewire/pulse doesn't hard-fail (was: EXIT_FAILURE with "Pipewire: Failed
    // to connect stream"). queue_samples/get_frames_remaining guard audio_device==0.
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (!reset_audio(48000)) {
        // Only unrecoverable converter init failure returns false now; missing device is silent.
        fprintf(stderr, "Could not set up audio, continuing without sound.\n");
    }

    // Source controller mappings file
    std::u8string controller_db_path = (recompui::file::get_program_path() / "recompcontrollerdb.txt").u8string();
    if (SDL_GameControllerAddMappingsFromFile(reinterpret_cast<const char *>(controller_db_path.c_str())) < 0) {
        fprintf(stderr, "Failed to load controller mappings: %s\n", SDL_GetError());
    }

    // Register fonts.
    recompui::register_primary_font("InterVariable.ttf", "Inter Variable");

    // Register configuration path.
    recomp::register_config_path(recompui::file::get_app_folder_path());

    // Register supported games and patches
    for (const auto& game : supported_games) {
        recomp::register_game(game);
    }


    REGISTER_FUNC(recomp_get_window_resolution);
    REGISTER_FUNC(recomp_get_target_aspect_ratio);
    REGISTER_FUNC(recomp_get_target_framerate);
    REGISTER_FUNC(recomp_get_right_analog_inputs);
    REGISTER_FUNC(recomp_get_camera_inputs);
    // REGISTER_FUNC(recomp_get_gyro_deltas);
    // REGISTER_FUNC(recomp_get_mouse_deltas);
    REGISTER_FUNC(recomp_get_level_select);
    REGISTER_FUNC(recomp_get_cheat_enabled);
    recompui::register_ui_exports();
    recomputil::register_data_api_exports();
    recomptheme::set_custom_theme();

    battletanx::register_btx_overlays();
    battletanx::register_btx_patches();

    // Register extensions for two types: Props and ActorMarkers.
    recomputil::init_extended_object_data(2);

    recompinput::players::set_single_player_mode(true);

    battletanx::init_config();

    recompui::register_launcher_init_callback(on_launcher_init);
    recompui::register_launcher_update_callback(on_launcher_update);

    recomp::rsp::callbacks_t rsp_callbacks{
        .get_rsp_microcode = get_rsp_microcode,
    };

    ultramodern::renderer::callbacks_t renderer_callbacks{
        .create_render_context = [](uint8_t* rdram, ultramodern::renderer::WindowHandle window_handle, bool developer_mode) {
#ifndef BTX_FORKED_RUNTIME
            btx_set_rdram(rdram);
#endif
            auto presentation_mode = ultramodern::renderer::PresentationMode::PresentEarly;
            return recompui::renderer::create_render_context(rdram, window_handle, presentation_mode, developer_mode);
        },
    };

    ultramodern::gfx_callbacks_t gfx_callbacks{
        .create_gfx = create_gfx,
        .create_window = create_window,
        .update_gfx = update_gfx,
    };

    ultramodern::audio_callbacks_t audio_callbacks{
        .queue_samples = queue_samples,
        .get_frames_remaining = get_frames_remaining,
        .set_frequency = set_frequency,
    };

    ultramodern::input::callbacks_t input_callbacks{
        .poll_input = recompinput::poll_inputs,
        .get_input = recompinput::profiles::get_n64_input,
        .set_rumble = recompinput::set_rumble,
        .get_connected_device_info = get_connected_device_info,
    };

    ultramodern::events::callbacks_t thread_callbacks{
        .vi_callback = recompinput::update_rumble,
        .gfx_init_callback = nullptr,
    };

    ultramodern::error_handling::callbacks_t error_handling_callbacks{
        .message_box = recompui::message_box,
    };

    ultramodern::threads::callbacks_t threads_callbacks{
        .get_game_thread_name = battletanx::get_game_thread_name,
    };

    // Register the texture pack content type with rt64.json as its content file.
    recomp::mods::ModContentType texture_pack_content_type{
        .content_filename = "rt64.json",
        .allow_runtime_toggle = true,
        .on_enabled = enable_texture_pack,
        .on_disabled = disable_texture_pack,
        .on_reordered = reorder_texture_pack,
    };
    auto texture_pack_content_type_id = recomp::mods::register_mod_content_type(texture_pack_content_type);

    // Register the .rtz texture pack file format with the previous content type as its only allowed content type.
    recomp::mods::register_mod_container_type("rtz", std::vector{ texture_pack_content_type_id }, false);

    recomp::start({
        .argc = argc,
        .argv = argv,
        .project_version = project_version,
        .rsp_callbacks = rsp_callbacks,
        .renderer_callbacks = renderer_callbacks,
        .audio_callbacks = audio_callbacks,
        .input_callbacks = input_callbacks,
        .gfx_callbacks = gfx_callbacks,
        .events_callbacks = thread_callbacks,
        .error_handling_callbacks = error_handling_callbacks,
        .threads_callbacks = threads_callbacks,
        // Upstream now drops PI completions that can't be delivered right away. The runtime used
        // to requeue them, so keep doing that.
        .message_queue_control = { .requeue_pi = true },
    });

    NFD_Quit();

    if (preloaded) {
        release_preload(preload_context);
    }

#ifdef _WIN32
    // End high resolution timing period.
    timeEndPeriod(1);
#endif

    // End the process without returning from main: the game's threads are never joined by the
    // runtime and may still be blocked on (or spinning over) runtime state, so running static
    // destructors / atexit handlers underneath them crashes on quit. Everything that must persist
    // has already been written (the saving thread is joined inside recomp::start).
    fflush(stdout);
    fflush(stderr);
    std::_Exit(EXIT_SUCCESS);
}

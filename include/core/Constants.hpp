#pragma once
#include <string_view>

namespace Config {

    namespace Game {
        // inline constexpr + string_view = Efficienza massima, zero allocazioni
        inline constexpr std::string_view kWindowName = "RPG Adventure";

        // Usiamo inline constexpr per i tipi primitivi
        inline constexpr int kWindowWidth = 800;
        inline constexpr int kWindowHeight = 600;
    
        inline constexpr std::string_view kFontName = "vt323";
        inline constexpr std::string_view kFontPath = "assets/fonts/VT323-Regular.ttf";

        inline constexpr std::string_view kMusicName = "menu_music";
        inline constexpr std::string_view kMusicPath = "assets/audio/menu_music.mp3";

        inline constexpr std::string_view kButtonHoverSfxName = "button_hover";
        inline constexpr std::string_view kButtonHoverSfxPath = "assets/audio/gui-wav/button_hover.wav";

        inline constexpr std::string_view kButtonClickSfxName = "button_click";
        inline constexpr std::string_view kButtonClickSfxPath = "assets/audio/gui-wav/button_click.wav";

        // Warmup OpenAL: riproduce un suono silenzioso all'avvio per pre-inizializzare il driver audio
        // ed eliminare il ritardo (~100ms) alla prima riproduzione. Disattivare se non necessario.
        inline constexpr bool kAudioWarmup = true;
    }

    namespace Graphics {
        inline constexpr unsigned int kTileSize = 64;
    }

    namespace MainMenu {
        inline constexpr int kBackgroundFrames = 48;
        inline constexpr float kFrameDuration = 0.1f; // 10 FPS

        inline constexpr std::string_view kBackgroundFrameName = "frame_";
        inline constexpr std::string_view kBackgroundFramePath = "assets/textures/main_menu/";
    }

    namespace Settings {
        inline constexpr int kVolumeStep = 25;
        inline constexpr int kMaxVolume = 100;
        inline constexpr int kMinVolume = 0;
        inline constexpr int kMusicVolume = 100;
        inline constexpr int kSoundVolume = 100;
    }
}
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

        // inline constexpr std::string_view kSoundName = "button_click";
        // inline constexpr std::string_view kSoundPath = "assets/audio/button_click.wav";
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
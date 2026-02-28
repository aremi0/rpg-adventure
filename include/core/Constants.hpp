#pragma once
#include <SFML/Graphics/Color.hpp>
#include <string_view>

namespace Config {

    namespace Game {
        // inline constexpr + string_view = Efficienza massima, zero allocazioni
        inline constexpr std::string_view kWindowName = "RPG Adventure";

        // Usiamo inline constexpr per i tipi primitivi
        inline constexpr int kWindowWidth = 1024;
        inline constexpr int kWindowHeight = 768;
    
        inline constexpr std::string_view kFontName = "vt323";
        inline constexpr std::string_view kFontPath = "assets/fonts/VT323-Regular.ttf";

        inline constexpr std::string_view kMusicName = "menu_music";
        inline constexpr std::string_view kMusicPath = "assets/audio/themes-ogg/ambient_3.ogg";

        inline constexpr std::string_view kButtonHoverSfxName = "button_hover";
        inline constexpr std::string_view kButtonHoverSfxPath = "assets/audio/gui-wav/button_hover.wav";

        inline constexpr std::string_view kButtonClickSfxName = "button_click";
        inline constexpr std::string_view kButtonClickSfxPath = "assets/audio/gui-wav/button_click.wav";

        // Warmup OpenAL: riproduce un suono silenzioso all'avvio per pre-inizializzare il driver audio
        inline constexpr bool kAudioWarmup = true;

        namespace Textures {
            inline constexpr unsigned int kTileSize = 64;

            inline constexpr std::string_view kHeroTexName = "hero_tex";
            inline constexpr std::string_view kHeroTexPath = "assets/textures/hero.png";
        }
    }

    namespace GUI {
        inline const sf::Color kIdleCol = sf::Color(70, 70, 70);
        inline const sf::Color kHoverCol = sf::Color(100, 100, 100);
        inline const sf::Color kActiveCol = sf::Color(40, 40, 40);

        inline const sf::Color kIdleRedCol = sf::Color(150, 50, 50);
        inline const sf::Color kHoverRedCol = sf::Color(200, 70, 70);
        inline const sf::Color kActiveRedCol = sf::Color(100, 30, 30);
    }

    namespace MainMenu {
        inline constexpr std::string_view kNuovaPartitaName = "Nuova Partita";
        inline constexpr std::string_view kImpostazioniName = "Impostazioni";
        inline constexpr std::string_view kEsciName = "Esci";

        inline constexpr int kBackgroundFrames = 48;
        inline constexpr float kFrameDuration = 0.1f; // 10 FPS

        inline constexpr std::string_view kBackgroundFrameName = "frame_";
        inline constexpr std::string_view kBackgroundFramePath = "assets/textures/main_menu/";
    }

    namespace Settings {
        inline constexpr std::string_view kRisoluzioneName = "Risoluzione";
        inline constexpr std::string_view kVolumeName = "Volume";
        inline constexpr std::string_view kIndietroName = "Indietro";

        inline constexpr int kVolumeStep = 25;
        inline constexpr int kMaxVolume = 100;
        inline constexpr int kMinVolume = 0;
        inline constexpr int kMusicVolume = 100;
        inline constexpr int kSoundVolume = 100;
    }
}
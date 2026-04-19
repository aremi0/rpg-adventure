#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Color.hpp>
#include <string_view>
#include <array>
#include <utility>
#include "utils/LogLevel.hpp"

namespace Config {

    namespace Game {
        // inline constexpr + string_view = Efficienza massima, zero allocazioni
        inline constexpr std::string_view kWindowName = "RPG Adventure";
    
        // Risoluzione logica del gioco (non la risoluzione della finestra)
        inline constexpr unsigned int kLogicalWidth = 1024;
        inline constexpr unsigned int kLogicalHeight = 768;

        inline constexpr std::string_view kFontName = "vt323";
        inline constexpr std::string_view kFontPath = "assets/fonts/VT323-Regular.ttf";

        inline constexpr std::string_view kMusicName = "menu_music";
        inline constexpr std::string_view kMusicPath = "assets/audio/themes-ogg/ambient_3.ogg";

        inline constexpr std::string_view kButtonHoverSfxName = "button_hover";
        inline constexpr std::string_view kButtonHoverSfxPath = "assets/audio/gui-wav/minimalist1_button_hover.wav";

        inline constexpr std::string_view kButtonClickSfxName = "button_click";
        inline constexpr std::string_view kButtonClickSfxPath = "assets/audio/gui-wav/african1_button_click.wav";

        // Warmup OpenAL: riproduce un suono silenzioso all'avvio per pre-inizializzare il driver audio
        inline constexpr bool kAudioWarmup = true;

        namespace Textures {
            inline constexpr unsigned int kTileSize = 64;

            inline constexpr std::string_view kHeroTexName = "hero_tex";
            inline constexpr std::string_view kHeroTexPath = "assets/textures/hero.png";
        }
    }

    namespace Debug {
        // Permette di attivare l'overlay visivo di debug in gioco
        inline constexpr bool kEnableVisualDebug = true;
        inline constexpr sf::Keyboard::Key kVisualDebugKey = sf::Keyboard::F1;

        // Permette di ciclare il log-level in gioco
        inline constexpr bool kEnableLogSwitching = true;
        inline constexpr sf::Keyboard::Key kLogSwitchKey = sf::Keyboard::F2;
        inline constexpr LogLevel kDefaultLogLevel = LogLevel::Debug;
    }

    namespace GUI {
        inline const sf::Color kIdleCol = sf::Color(70, 70, 70);
        inline const sf::Color kHoverCol = sf::Color(100, 100, 100);
        inline const sf::Color kActiveCol = sf::Color(40, 40, 40);

        inline const sf::Color kIdleRedCol = sf::Color(150, 50, 50);
        inline const sf::Color kHoverRedCol = sf::Color(200, 70, 70);
        inline const sf::Color kActiveRedCol = sf::Color(100, 30, 30);

        inline const sf::Color kLabelCol = sf::Color(50, 50, 50);

        inline const sf::Color kIdleGreenCol = sf::Color(0, 155, 85);
        inline const sf::Color kHoverGreenCol = sf::Color(0, 190, 105);
        inline const sf::Color kActiveGreenCol = sf::Color(0, 120, 65);
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
        // Risoluzione di default della FINESTRA (non la risoluzione logica!)
        inline constexpr unsigned int kDefaultWindowWidth = 1024;
        inline constexpr unsigned int kDefaultWindowHeight = 768;

        // Risoluzioni finestra supportate (whitelist per validazione)
        inline constexpr std::array<std::pair<unsigned int, unsigned int>, 3> kSupportedResolutions = {{
            {800, 600},
            {1024, 768},
            {1280, 720}
        }};

        inline constexpr std::string_view kRisoluzioneName = "Risoluzione";
        inline constexpr std::string_view kIndietroName = "Indietro";

        // Nomi per i bottoni volume
        inline constexpr std::string_view kMasterVolumeName = "Master";
        inline constexpr std::string_view kMusicVolumeName  = "Musica";
        inline constexpr std::string_view kSfxVolumeName    = "Effetti";
        inline constexpr std::string_view kUiVolumeName     = "Interfaccia";

        // Volumi default per canale (0-100)
        inline constexpr int kDefaultMasterVolume = 100;
        inline constexpr int kDefaultMusicVolume  = 25;
        inline constexpr int kDefaultSfxVolume    = 100;
        inline constexpr int kDefaultUiVolume     = 100;

        inline constexpr int kVolumeStep = 5;
        inline constexpr int kMaxVolume = 100;
        inline constexpr int kMinVolume = 0;

        inline constexpr std::string_view kArrowLeft = "<";
        inline constexpr std::string_view kArrowRight = ">";
    }
}
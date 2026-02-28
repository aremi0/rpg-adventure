#include "states/SettingsState.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include <SFML/Audio/Listener.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <format>
#include <SFML/Audio.hpp>

SettingsState::SettingsState(GameDataRef data)
    : State("SettingsState"), data_(data), volume_level_(Config::Settings::kMaxVolume), res_index_(1) {
        supported_resolutions_ = {
            sf::VideoMode(800, 600),
            sf::VideoMode(Config::Game::kWindowWidth, Config::Game::kWindowHeight),
            sf::VideoMode(1280, 720)
        };
    }

void SettingsState::Init() {
    Logger::Debug("Inizializzazione ({})", this->GetStateName());

    // ----------------- Loading Settings background
    // Attualmente lo sfondo è semplicemente un overlay del MainMenuState con blur (Guarda metodo Draw() più giù)
/*     auto bg_result = data_->assets.LoadAsset<sf::Texture>(Config::Settings::kBackgroundName, Config::Settings::kBackgroundPath);
    if (bg_result) {
        // 1. Se il caricamento va a buon fine, impostiamo la texture
        const auto& bg_tex = data_->assets.GetAsset<sf::Texture>(Config::Settings::kBackgroundName);
        background_sprite_.setTexture(bg_tex);

        // 2. Adattiamo lo sfondo alla finestra (opzionale)
        sf::Vector2u tex_size = bg_tex.getSize();
        sf::Vector2u win_size = data_->window.getSize();
        background_sprite_.setScale(
            static_cast<float>(win_size.x) / tex_size.x, 
            static_cast<float>(win_size.y) / tex_size.y
        );
    } else {
        Logger::Error("Errore caricamento sfondo '{}' da: {}", 
            Config::Settings::kBackgroundName, Config::Settings::kBackgroundPath);
    } */
    
    // ----------------- Creating Settings buttons
    const sf::Font& font = data_->assets.GetAsset<sf::Font>(std::string(Config::Game::kFontName));

    // Centro della finestra per allineare i bottoni
    float center_x = data_->window.getSize().x / 2.0f - 150.0f; // 150 è metà della larghezza del bottone
    float start_y = 250.0f;
    float spacing = 80.0f;

    // Bottone Risoluzione
    res_button_ = std::make_unique<Button>(
        center_x, start_y, 300.0f, 50.0f, font, 
        std::format("{}: {}x{}", 
                    Config::Settings::kRisoluzioneName, 
                    supported_resolutions_[res_index_].width, 
                    supported_resolutions_[res_index_].height),
        24, Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol
    );

    // Bottone Volume
    vol_button_ = std::make_unique<Button>(
        center_x, start_y + spacing, 300.0f, 50.0f, font, 
        std::format("{}: {}%", 
                    Config::Settings::kVolumeName, 
                    volume_level_),
        24, Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol
    );

    // Bottone Indietro
    back_button_ = std::make_unique<Button>(
        center_x, start_y + spacing * 2, 300.0f, 50.0f, font, 
        std::format("{}", Config::Settings::kIndietroName), 24, 
        Config::GUI::kIdleRedCol, Config::GUI::kHoverRedCol, Config::GUI::kActiveRedCol
    );
}

void SettingsState::HandleInput() {
    sf::Event event;
    while (data_->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            data_->window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            Logger::Debug("Tasto premuto: ({})", (int)event.key.code);
        }
    }
}

void SettingsState::Update(float dt) {
    // Mappiamo le coordinate del mouse rispetto alla finestra
    sf::Vector2f mouse_pos = data_->window.mapPixelToCoords(sf::Mouse::getPosition(data_->window));

    // Aggiorniamo lo stato dei bottoni
    res_button_->Update(mouse_pos);
    vol_button_->Update(mouse_pos);
    back_button_->Update(mouse_pos);

    // Logica dei click
    if (res_button_->IsPressed()) {
        CycleResolution();
    }

    if (vol_button_->IsPressed()) {
        CycleVolume();
    }

    if (back_button_->IsPressed()) {
        Logger::Trace("Ritorno al menu principale");
        data_->machine.RemoveState(); // Toglie SettingsState e fa il Resume di MainMenuState
    }
}

void SettingsState::Draw() {
    // Usiamo le costanti della Risoluzione Logica invece della grandezza fisica della finestra
    sf::RectangleShape overlay(sf::Vector2f(
        static_cast<float>(Config::Game::kWindowWidth), 
        static_cast<float>(Config::Game::kWindowHeight)
    ));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    data_->window.draw(overlay);

    // Disegniamo i bottoni
    res_button_->Render(data_->window);
    vol_button_->Render(data_->window);
    back_button_->Render(data_->window);
}

void SettingsState::CycleVolume() {
    volume_level_ -= Config::Settings::kVolumeStep;
    if (volume_level_ < Config::Settings::kMinVolume) {
        volume_level_ = Config::Settings::kMaxVolume;
    }

    // SFML gesatisce il volume globale (0.0f - 100.0f)
    sf::Listener::setGlobalVolume(static_cast<float>(volume_level_));
    Logger::Info("Volume impostato a: {}%", volume_level_);

    vol_button_->SetText(std::format("{}: {}%", 
        Config::Settings::kVolumeName, 
        volume_level_));
}

void SettingsState::CycleResolution() {
    res_index_ = (res_index_ + 1) % supported_resolutions_.size();
    auto new_mode = supported_resolutions_[res_index_];

    // Ricrea la finestra con la nuova risoluzione
    data_->window.create(new_mode, std::string(Config::Game::kWindowName));

    sf::View view(sf::FloatRect(0, 0, Config::Game::kWindowWidth, Config::Game::kWindowHeight));
    data_->window.setView(view);

    Logger::Info("Risoluzione impostata a: {}x{}", new_mode.width, new_mode.height);
    res_button_->SetText(std::format("{}: {}x{}", 
        Config::Settings::kRisoluzioneName, 
        new_mode.width, 
        new_mode.height));
}
#include "states/MainMenuState.hpp"
#include "states/GameState.hpp"
#include "states/SettingsState.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <format>

MainMenuState::MainMenuState(GameDataRef data) 
    : State("MainMenuState"),
    data_(data),
    background_(std::make_unique<AnimatedBackground>(data)) {}

void MainMenuState::Init() {
    Logger::Debug("Inizializzazione ({})", this->GetStateName());

    // 1. Provo a caricare una texture che non esiste (Test Fallback)
    auto texture_error_test = data_->assets.LoadAsset<sf::Texture>("hero_sprite", "assets/textures/non_esiste.png");
    if (!texture_error_test) {
        Logger::Error("Caricamento fallito come previsto (Test Fallback). Errore: {}", static_cast<int>(texture_error_test.error()));
    }

    // 2. Recupero la texture (dovrebbe restituire quella viola/nera a scacchi)
    const sf::Texture& fallback_texture_test = data_->assets.GetAsset<sf::Texture>("hero_sprite");

    // 3. Configuro uno sprite di test per vederla
    test_sprite_.setTexture(fallback_texture_test);
    test_sprite_.setPosition(100, 100);
    test_sprite_.setScale(2.0f, 2.0f);

    // ----------------- Loading MainMenu background frames
    // 1. Carico i frame del background
    for (int i = 0; i < Config::MainMenu::kBackgroundFrames; ++i) {
        // Genera i nomi
        std::string texture_name = std::format("{}{}", Config::MainMenu::kBackgroundFrameName, i);
        std::string file_path = std::format("{}{}.png", Config::MainMenu::kBackgroundFramePath, texture_name);

        // Carica i frame
        auto bg_frame_result = data_->assets.LoadAsset<sf::Texture>(texture_name, file_path);
        
        // Aggiungi al background (AnimatedBackground gestisce internamente se la texture è valida)
        // Nota: LoadAsset ritorna void se successo, o errore. 
        // Se ha successo, la texture è nel manager e possiamo aggiungere il NOME al background.
        if (bg_frame_result) {
            background_->AddFrame(texture_name);
        }
    }

    // 2. Adatto background alla finestra
    background_->Resize(sf::Vector2u(Config::Game::kWindowWidth, Config::Game::kWindowHeight));
    
    // ----------------- Loading Game musics
    auto music_res = data_->assets.LoadMusic(
        std::string(Config::Game::kMusicName), 
        std::string(Config::Game::kMusicPath)
    );
    if (music_res) {
        sf::Music& music = data_->assets.GetMusic(std::string(Config::Game::kMusicName));
        music.setLoop(true);
        music.setVolume(static_cast<float>(Config::Settings::kMusicVolume));
        music.play();
    }

    // ----------------- Creating MainMenu buttons
    // ----------------- Loading Game fonts
    auto font_res = data_->assets.LoadAsset<sf::Font>(
        std::string(Config::Game::kFontName), 
        std::string(Config::Game::kFontPath)
    );
    if (font_res) {
        const sf::Font& font = data_->assets.GetAsset<sf::Font>(std::string(Config::Game::kFontName));
        
        // Posizionamento dinamico in base alla finestra
        float center_x = data_->window.getSize().x / 2.0f - 150.0f; // 150 = metà di 300 (larghezza)
        float start_y = 250.0f;
        float spacing = 80.0f;

        play_button_ = std::make_unique<Button>(
            center_x, start_y, 300.0f, 50.0f,
            font, std::string(Config::MainMenu::kNuovaPartitaName), 
            24, Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol
        );

        settings_button_ = std::make_unique<Button>(
            center_x, start_y + spacing, 300.0f, 50.0f,
            font, std::string(Config::MainMenu::kImpostazioniName), 
            24, Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol
        );

        exit_button_ = std::make_unique<Button>(
            center_x, start_y + spacing * 2, 300.0f, 50.0f,
            font, std::string(Config::MainMenu::kEsciName), 
            24, Config::GUI::kIdleRedCol, Config::GUI::kHoverRedCol, Config::GUI::kActiveRedCol
        );
    }
}

void MainMenuState::HandleInput() {
    sf::Event event;
    while (data_->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            data_->window.close();
        }
        
        // Esempio: premi un tasto per loggare qualcosa
        if (event.type == sf::Event::KeyPressed) {
            Logger::Debug("Tasto premuto: ({})", (int)event.key.code);
        }
    }
}

void MainMenuState::Update(float dt) {
    // Mappiamo le coordinate del mouse rispetto alla finestra
    sf::Vector2f mouse_pos = data_->window.mapPixelToCoords(sf::Mouse::getPosition(data_->window));

    // Aggiorniamo lo stato dei bottoni
    play_button_->Update(mouse_pos);
    settings_button_->Update(mouse_pos);
    exit_button_->Update(mouse_pos);

    // Logica dei Click
    if (play_button_->IsPressed()) {
        Logger::Info("Avvio Nuova Partita...");
        // Qui in futuro metteremo: 
        data_->machine.AddState(std::make_unique<GameState>(data_), true);
    }

    if (settings_button_->IsPressed()) {
        Logger::Info("Apertura Impostazioni...");
        // Pushiamo le impostazioni in OVERLAY (is_replacing = false)
        data_->machine.AddState(std::make_unique<SettingsState>(data_), false);
    }

    if (exit_button_->IsPressed()) {
        Logger::Info("Uscita dal gioco in corso.");
        data_->machine.RemoveState();
    }
}

void MainMenuState::UpdateVisuals(float dt) {
    background_->Update(dt);
}

void MainMenuState::Draw() {
    background_->Draw();
    

    if (!is_paused_) {
        data_->window.draw(test_sprite_);
        
        // Disegniamo i bottoni
        play_button_->Render(data_->window);
        settings_button_->Render(data_->window);
        exit_button_->Render(data_->window);
    }

}

void MainMenuState::Pause() {
    is_paused_ = true;
    Logger::Trace("{} messo in pausa (Nascondo bottoni)", this->GetStateName());
}

void MainMenuState::Resume() {
    is_paused_ = false;
    Logger::Trace("{} messo in ripresa (Mostro bottoni)", this->GetStateName());
}
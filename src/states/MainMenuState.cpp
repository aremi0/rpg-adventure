#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <format>

MainMenuState::MainMenuState(GameDataRef data,  std::string_view state_name) 
    : data_(data), 
    State(state_name),
    background_(std::make_unique<AnimatedBackground>(data))
{
    background_->SetFrameDuration(Config::MainMenu::kFrameDuration);
}

void MainMenuState::Init() {
    Logger::Trace("Inizializzazione ({})", this->GetStateName());

    // 1. Provo a caricare una texture che non esiste (Test Fallback)
    auto result = data_->assets.LoadAsset<sf::Texture>("hero_sprite", "assets/textures/non_esiste.png");
    if (!result) {
        Logger::Error("Caricamento fallito come previsto (Test Fallback). Errore: {}", static_cast<int>(result.error()));
    }

    // 2. Recupero la texture (dovrebbe restituire quella viola/nera a scacchi)
    const sf::Texture& tex = data_->assets.GetAsset<sf::Texture>("hero_sprite");

    // 3. Configuro uno sprite di test per vederla
    test_sprite_.setTexture(tex);
    test_sprite_.setPosition(100, 100);
    test_sprite_.setScale(2.0f, 2.0f);

    // ----------------- Background

    // 1. Carico i frame del background
    for (int i = 0; i < Config::MainMenu::kBackgroundFrames; ++i) {
        // Genera i nomi
        std::string texture_name = std::format("frame_{}", i);
        std::string file_path = std::format("assets/textures/main_menu/{}.png", texture_name);

        // Carica
        auto res = data_->assets.LoadAsset<sf::Texture>(texture_name, file_path);
        
        // Aggiungi al background (AnimatedBackground gestisce internamente se la texture è valida)
        // Nota: LoadAsset ritorna void se successo, o errore. 
        // Se ha successo, la texture è nel manager e possiamo aggiungere il NOME al background.
        if (res) {
            background_->AddFrame(texture_name);
        } else {
            Logger::Error("Errore caricamento frame sfondo: {}", texture_name);
        }
    }

    // Adatta alla finestra
    background_->Resize(data_->window.getSize());
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
    background_->Update(dt);
}

void MainMenuState::Draw() {
    data_->window.clear();
    background_->Draw();
    data_->window.draw(test_sprite_);
    data_->window.display();
}
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

MainMenuState::MainMenuState(GameDataRef data,  std::string_view state_name) : data_(data), State(state_name) {}

void MainMenuState::Init() {
    Logger::Trace("Inizializzazione ({})", this->GetStateName());

    // 1. Provo a caricare una texture che non esiste
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

    auto main_menu_background = data_->assets.LoadAsset<sf::Texture>("background", "assets/textures/main_menu.gif");
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
    // Logica del menu (es. animazioni)
}

void MainMenuState::Draw() {
    data_->window.draw(test_sprite_);
}
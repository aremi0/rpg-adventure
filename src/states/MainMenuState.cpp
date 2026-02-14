#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

MainMenuState::MainMenuState(GameDataRef data,  std::string_view state_name) : data_(data), State(state_name) {}

void MainMenuState::Init() {
    Logger::Trace("Inizializzazione ({})", this->GetStateName());

    test_button_.setSize({200.0f, 50.0f});
    test_button_.setFillColor(sf::Color::Red);
    test_button_.setPosition({300.0f, 275.0f}); // Al centro circa
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
    data_->window.draw(test_button_);
}
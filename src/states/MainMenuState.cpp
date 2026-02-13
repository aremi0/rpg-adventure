#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

MainMenuState::MainMenuState(GameDataRef data) : _data(data) {}

void MainMenuState::init() {
    Logger::trace("Inizializzazione MainMenuState...");
    
    _testButton.setSize({200.0f, 50.0f});
    _testButton.setFillColor(sf::Color::Red);
    _testButton.setPosition({300.0f, 275.0f}); // Al centro circa
}

void MainMenuState::handleInput() {
    sf::Event event;
    while (_data->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _data->window.close();
        }
        
        // Esempio: premi un tasto per loggare qualcosa
        if (event.type == sf::Event::KeyPressed) {
            Logger::debug("Tasto premuto: {}", (int)event.key.code);
        }
    }
}

void MainMenuState::update(float dt) {
    // Logica del menu (es. animazioni)
}

void MainMenuState::draw() {
    _data->window.draw(_testButton);
}
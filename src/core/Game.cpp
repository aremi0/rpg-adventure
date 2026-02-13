#include "core/Game.hpp"
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

Game::Game(int width, int height, std::string title) : _data(std::make_shared<GameData>()) {
    _data->window.create(sf::VideoMode({static_cast<unsigned>(width), static_cast<unsigned>(height)}), title);
    
    // Qui in futuro caricheremo lo stato iniziale (es. MenuState)
    _data->machine.addState(std::make_unique<MainMenuState>(_data));
    Logger::debug("Stato iniziale caricato: Main Menu");

    this->run();
}   

void Game::run() {
    float newTime, frameTime, interpolation;
    float currentTime = _clock.getElapsedTime().asSeconds();
    float accumulator = 0.0f;

    while (_data->window.isOpen()) {
        // Gestione cambi di stato
        _data->machine.processStateChanges();

        newTime = _clock.getElapsedTime().asSeconds();
        frameTime = newTime - currentTime;

        if (frameTime > 0.25f) frameTime = 0.25f; // Limite per evitare "salti" enormi dopo un lag

        currentTime = newTime;
        accumulator += frameTime;

        // Loop di Update (Fixed Timestep)
        while (accumulator >= dt) {
            _data->machine.getActiveState()->handleInput();
            _data->machine.getActiveState()->update(dt);

            accumulator -= dt;
        }

        // Render
        _data->window.clear();
        _data->machine.getActiveState()->draw();
        _data->window.display();
    }
}
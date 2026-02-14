#include "core/Game.hpp"
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

Game::Game(int width, int height, std::string title) : data_(std::make_shared<GameData>()) {
    data_->window.create(sf::VideoMode({static_cast<unsigned>(width), static_cast<unsigned>(height)}), title);
    
    // Qui in futuro caricheremo lo stato iniziale (es. MenuState)
    data_->machine.AddState(std::make_unique<MainMenuState>(data_, "Main Menu"));
    data_->machine.ProcessStateChanges();
    Logger::Debug("Stato iniziale caricato: ({})", data_->machine.GetActiveState()->GetStateName());

    this->Run();
}   

void Game::Run() {
    float new_time, frame_time, interpolation;
    float current_time = clock_.getElapsedTime().asSeconds();
    float accumulator = 0.0f;

    while (data_->window.isOpen()) {
        // Gestione cambi di stato
        data_->machine.ProcessStateChanges();

        new_time = clock_.getElapsedTime().asSeconds();
        frame_time = new_time - current_time;

        if (frame_time > 0.25f) frame_time = 0.25f; // Limite per evitare "salti" enormi dopo un lag

        current_time = new_time;
        accumulator += frame_time;

        // Loop di Update (Fixed Timestep)
        while (accumulator >= kDeltaTime) {
            data_->machine.GetActiveState()->HandleInput();
            data_->machine.GetActiveState()->Update(kDeltaTime);

            accumulator -= kDeltaTime;
        }

        // Render
        data_->window.clear();
        data_->machine.GetActiveState()->Draw();
        data_->window.display();
    }
}
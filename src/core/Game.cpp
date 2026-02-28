#include "core/Game.hpp"
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include <SFML/Audio.hpp>

Game::Game() : data_(std::make_shared<GameData>()) {
    // Crea la finestra di gioco
    data_->window.create(
        sf::VideoMode(
            {
                static_cast<unsigned>(Config::Game::kWindowWidth), 
                static_cast<unsigned>(Config::Game::kWindowHeight)
            }), 
        std::string(Config::Game::kWindowName));

    // Warmup OpenAL: pre-inizializza il driver audio per eliminare la latenza al primo suono
    if constexpr (Config::Game::kAudioWarmup) {
        sf::SoundBuffer silent_buf;
        // Creiamo un buffer con un singolo campione silenzioso (16-bit, mono, 44100Hz)
        const sf::Int16 silence = 0;
        silent_buf.loadFromSamples(&silence, 1, 1, 44100);
        sf::Sound warmup;
        warmup.setBuffer(silent_buf);
        warmup.setVolume(0.f);
        warmup.play();
        sf::sleep(sf::milliseconds(10));
        warmup.stop();
        Logger::Trace("Audio warmup completato (OpenAL pre-inizializzato)");
    }

    // Qui in futuro caricheremo lo stato iniziale (es. MenuState)
    data_->machine.AddState(std::make_unique<MainMenuState>(data_));
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

        if (data_->machine.IsEmpty()) {
            Logger::Warn("Tutti gli stati sono stati rimossi. Chiudo il gioco.");
            data_->window.close();
            break;
        }

        new_time = clock_.getElapsedTime().asSeconds();
        frame_time = new_time - current_time;

        if (frame_time > 0.25f) frame_time = 0.25f; // Limite per evitare "salti" enormi dopo un lag

        current_time = new_time;
        accumulator += frame_time;

        // Loop di Update (Fixed Timestep)
        while (accumulator >= kDeltaTime) {
            // Update di Logica e Input solo per lo State attivo
            data_->machine.GetActiveState()->HandleInput();
            data_->machine.GetActiveState()->Update(kDeltaTime);

            // Update della Grafica (animazioni ambientazione) per tutti gli stati nello stack
            for (auto& state : data_->machine.GetStates()) {
                state->UpdateVisuals(kDeltaTime);
            }

            accumulator -= kDeltaTime;
        }

        // Render
        data_->window.clear();
        data_->machine.Draw();
        data_->window.display();
    }
}
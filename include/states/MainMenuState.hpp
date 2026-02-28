#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"
#include "ui/AnimatedBackground.hpp"
#include "ui/Button.hpp"

class MainMenuState : public State {
    public:
        MainMenuState(GameDataRef data);

        void Init() override;
        void HandleInput() override;
        void Update(float dt) override;
        void UpdateVisuals(float dt) override;
        void Draw() override;

        // Metodi per gestire l'overlay
        void Pause() override;
        void Resume() override;

    private:
        GameDataRef data_;
        bool is_paused_ = false;

        std::unique_ptr<AnimatedBackground> background_; // Variabile per l'animazione del background
        sf::Sprite test_sprite_;

        std::unique_ptr<Button> play_button_;
        std::unique_ptr<Button> settings_button_;
        std::unique_ptr<Button> exit_button_;
};
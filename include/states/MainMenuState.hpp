#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"
#include "ui/AnimatedBackground.hpp"

class MainMenuState : public State {
    public:
        MainMenuState(GameDataRef data, std::string_view state_name);

        void Init() override;
        void HandleInput() override;
        void Update(float dt) override;
        void Draw() override;

    private:
        GameDataRef data_;

        // --- Variabile per l'animazione del background ---
        std::unique_ptr<AnimatedBackground> background_;

        sf::Sprite test_sprite_;
};
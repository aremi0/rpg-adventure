#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"

class MainMenuState : public State {
public:
    MainMenuState(GameDataRef data, std::string_view state_name);

    void Init() override;
    void HandleInput() override;
    void Update(float dt) override;
    void Draw() override;

private:
    GameDataRef data_;
    sf::RectangleShape test_button_; // Solo per vedere qualcosa a schermo
};
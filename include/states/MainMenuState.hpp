#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"

class MainMenuState : public State {
public:
    MainMenuState(GameDataRef data);

    void init() override;
    void handleInput() override;
    void update(float dt) override;
    void draw() override;

private:
    GameDataRef _data;
    sf::RectangleShape _testButton; // Solo per vedere qualcosa a schermo
};
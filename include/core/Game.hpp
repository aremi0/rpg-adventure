#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include <string>

struct GameData {
    sf::RenderWindow window;
    StateMachine machine;
    // Qui aggiungeremo in futuro AssetManager e InputManager
};

using GameDataRef = std::shared_ptr<GameData>;

class Game {
public:
    Game(int width, int height, std::string title);

private:
    static constexpr float dt = 1.0f / 60.0f; // Clock fisso a 60 FPS per la logica
    sf::Clock _clock;
    GameDataRef _data;

    void run();
};
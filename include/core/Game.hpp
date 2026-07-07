#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include "resources/AssetManager.hpp"
#include <entt/entt.hpp>
#include "core/ConfigManager.hpp"
#include "map/MapManager.hpp"

struct GameData {
    sf::RenderWindow window;
    StateMachine machine;
    AssetManager assets;
    entt::registry registry;
    ConfigManager config;
    MapManager map;
};

using GameDataRef = std::shared_ptr<GameData>;

class Game {
    public:
        Game();

    private:
        static constexpr float kDeltaTime = 1.0f / 60.0f; // Clock fisso a 60 FPS per la logica
        sf::Clock clock_;
        GameDataRef data_;

        void Run();
};
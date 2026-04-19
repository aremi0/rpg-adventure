#pragma once
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

class DebugRenderSystem {
    public:
        // Disegna le informazioni di debug per tutte le entità che hanno un Transform
        static void Draw(entt::registry& registry, sf::RenderTarget& target, const sf::Font& font);
};

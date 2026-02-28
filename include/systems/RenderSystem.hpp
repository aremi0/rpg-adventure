#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <entt/entt.hpp>

class RenderSystem {
        public:
            // Passiamo il registry (il database) e il target (la finestra dove disegnare)
            static void Draw(entt::registry& registry, sf::RenderTarget& target);
};
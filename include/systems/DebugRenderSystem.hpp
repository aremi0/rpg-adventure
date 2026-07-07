#pragma once
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

class MapManager;

class DebugRenderSystem {
 public:
  static void Draw(entt::registry& registry, sf::RenderTarget& target,
                   const sf::Font& font, const MapManager* map = nullptr);
};

#pragma once

#include <entt/entt.hpp>

class MapManager;

class CollisionSystem {
 public:
  static void Update(entt::registry& registry, const MapManager& map, float dt);
};

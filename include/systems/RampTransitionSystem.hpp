#pragma once

#include <entt/entt.hpp>

class MapManager;

class RampTransitionSystem {
 public:
  static void Update(entt::registry& registry, const MapManager& map, float dt);
};

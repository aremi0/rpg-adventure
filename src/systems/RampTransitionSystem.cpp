#include "systems/RampTransitionSystem.hpp"

#include <algorithm>
#include <cmath>

#include "components/Components.hpp"
#include "core/Constants.hpp"
#include "map/LdtkTypes.hpp"
#include "map/MapCollisionUtils.hpp"
#include "map/MapManager.hpp"

void RampTransitionSystem::Update(entt::registry& registry,
                                  const MapManager& map, const float dt) {
  const auto& level = map.GetLevel();
  const int tile_size = GetTileSize(level);

  auto view = registry.view<TransformComponent, ElevationComponent,
                            BoxColliderComponent>();

  for (const auto entity : view) {
    const auto& transform = view.get<TransformComponent>(entity);
    auto& elevation = view.get<ElevationComponent>(entity);
    const auto& collider = view.get<BoxColliderComponent>(entity);

    const sf::FloatRect foot = GetFootAabb(transform, collider);
    const float foot_x = foot.left + foot.width * 0.5f;
    const float foot_y = foot.top + foot.height * 0.5f;

    int tx = 0;
    int ty = 0;
    WorldToTile(foot_x, foot_y, level, tx, ty);

    const CollisionType collision = map.CollisionAt(tx, ty);
    float target_height = 0.f;

    if (collision != CollisionType::kStairs) {
      elevation.floor_level = map.ElevationAt(tx, ty);
      target_height = static_cast<float>(elevation.floor_level) *
                      Config::Map::kHeightPerLevel;
    } else {
      const int to_level = map.ElevationAt(tx, ty);
      const int from_level = elevation.floor_level;
      const float tile_top_y =
          static_cast<float>(level.world_y + ty * tile_size);
      const float ramp_t = std::clamp(
          (foot_y - tile_top_y) / static_cast<float>(tile_size), 0.f, 1.f);
      const float from_height =
          static_cast<float>(from_level) * Config::Map::kHeightPerLevel;
      const float to_height =
          static_cast<float>(to_level) * Config::Map::kHeightPerLevel;
      target_height = std::lerp(from_height, to_height, ramp_t);
      if (ramp_t >= 0.5f) {
        elevation.floor_level = to_level;
      }
    }

    const float lerp_factor =
        std::min(1.f, Config::Map::kHeightLerpSpeed * dt);
    elevation.height = std::lerp(elevation.height, target_height, lerp_factor);
  }
}

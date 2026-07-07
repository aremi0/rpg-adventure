#include "systems/CollisionSystem.hpp"

#include <cmath>

#include <SFML/Graphics/Rect.hpp>

#include "components/Components.hpp"
#include "map/MapCollisionUtils.hpp"
#include "map/LdtkTypes.hpp"
#include "map/MapManager.hpp"

namespace {

bool IsCollisionBlocking(CollisionType type) {
  return type == CollisionType::kBlocked || type == CollisionType::kWater;
}

bool IsBlocked(const sf::FloatRect& aabb, const MapManager& map) {
  const auto& level = map.GetLevel();
  const int tile_size = GetTileSize(level);
  const float world_x = static_cast<float>(level.world_x);
  const float world_y = static_cast<float>(level.world_y);

  const int min_tx = static_cast<int>(
      std::floor((aabb.left - world_x) / static_cast<float>(tile_size)));
  const int max_tx = static_cast<int>(std::floor(
      (aabb.left + aabb.width - world_x - 0.001f) /
      static_cast<float>(tile_size)));
  const int min_ty = static_cast<int>(
      std::floor((aabb.top - world_y) / static_cast<float>(tile_size)));
  const int max_ty = static_cast<int>(std::floor(
      (aabb.top + aabb.height - world_y - 0.001f) /
      static_cast<float>(tile_size)));

  for (int ty = min_ty; ty <= max_ty; ++ty) {
    for (int tx = min_tx; tx <= max_tx; ++tx) {
      if (IsCollisionBlocking(map.CollisionAt(tx, ty))) {
        return true;
      }
    }
  }
  return false;
}

bool TryMoveAxis(TransformComponent& transform,
                 const BoxColliderComponent& collider, float delta,
                 bool move_x, const MapManager& map) {
  TransformComponent candidate = transform;
  if (move_x) {
    candidate.position.x += delta;
  } else {
    candidate.position.y += delta;
  }

  if (!IsBlocked(GetFootAabb(candidate, collider), map)) {
    transform.position = candidate.position;
    return true;
  }
  return false;
}

}  // namespace

void CollisionSystem::Update(entt::registry& registry, const MapManager& map,
                             const float dt) {
  auto view = registry.view<TransformComponent, VelocityComponent,
                            BoxColliderComponent>();

  for (const auto entity : view) {
    auto& transform = view.get<TransformComponent>(entity);
    const auto& velocity = view.get<VelocityComponent>(entity);
    const auto& collider = view.get<BoxColliderComponent>(entity);

    if (collider.is_trigger) {
      continue;
    }

    const sf::Vector2f delta = velocity.velocity * dt;
    if (delta.x != 0.f) {
      TryMoveAxis(transform, collider, delta.x, true, map);
    }
    if (delta.y != 0.f) {
      TryMoveAxis(transform, collider, delta.y, false, map);
    }
  }
}

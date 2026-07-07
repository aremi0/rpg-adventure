#include "map/MapCollisionUtils.hpp"

#include <array>
#include <cmath>
#include <queue>
#include <utility>
#include <vector>

#include "core/Constants.hpp"
#include "map/MapManager.hpp"

namespace {

constexpr std::array<std::pair<int, int>, 4> kCardinalNeighbors = {
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

}  // namespace

int GetTileSize(const LdtkLevel& level) {
  return level.tile_size > 0 ? level.tile_size : Config::Map::kTileSize;
}

sf::FloatRect GetFootAabb(const TransformComponent& transform,
                          const BoxColliderComponent& collider) {
  const sf::Vector2f center = transform.position + collider.offset;
  return {center.x - collider.size.x * 0.5f, center.y - collider.size.y * 0.5f,
          collider.size.x, collider.size.y};
}

void WorldToTile(const float world_x, const float world_y,
                 const LdtkLevel& level, int& tx, int& ty) {
  const int tile_size = GetTileSize(level);
  tx = static_cast<int>(std::floor(
      (world_x - static_cast<float>(level.world_x)) /
      static_cast<float>(tile_size)));
  ty = static_cast<int>(std::floor(
      (world_y - static_cast<float>(level.world_y)) /
      static_cast<float>(tile_size)));
}

sf::Vector2f GetTileCenter(const int tx, const int ty,
                           const LdtkLevel& level) {
  const int tile_size = GetTileSize(level);
  const float half = static_cast<float>(tile_size) * 0.5f;
  return {static_cast<float>(level.world_x + tx * tile_size) + half,
          static_cast<float>(level.world_y + ty * tile_size) + half};
}

bool IsPassableCollision(const CollisionType type) {
  return type == CollisionType::kWalkable || type == CollisionType::kStairs;
}

bool IsValidSpawnTile(const MapManager& map, const int tx, const int ty) {
  if (!IsPassableCollision(map.CollisionAt(tx, ty))) {
    return false;
  }

  for (const auto [dx, dy] : kCardinalNeighbors) {
    if (IsPassableCollision(map.CollisionAt(tx + dx, ty + dy))) {
      return true;
    }
  }
  return false;
}

std::optional<sf::Vector2f> FindNearbySpawnPosition(const MapManager& map,
                                                    const float world_x,
                                                    const float world_y) {
  const auto& level = map.GetLevel();
  int start_tx = 0;
  int start_ty = 0;
  WorldToTile(world_x, world_y, level, start_tx, start_ty);

  if (IsValidSpawnTile(map, start_tx, start_ty)) {
    return GetTileCenter(start_tx, start_ty, level);
  }

  struct TileCoord {
    int x;
    int y;
  };

  std::queue<TileCoord> queue;
  std::vector<std::vector<bool>> visited(
      static_cast<std::size_t>(Config::Map::kMaxSpawnSearchRadius * 2 + 1),
      std::vector<bool>(
          static_cast<std::size_t>(Config::Map::kMaxSpawnSearchRadius * 2 + 1),
          false));
  const int origin_x = start_tx;
  const int origin_y = start_ty;

  auto mark_visited = [&](const int x, const int y) {
    const int local_x = x - origin_x + Config::Map::kMaxSpawnSearchRadius;
    const int local_y = y - origin_y + Config::Map::kMaxSpawnSearchRadius;
    if (local_x < 0 || local_y < 0 ||
        local_x > Config::Map::kMaxSpawnSearchRadius * 2 ||
        local_y > Config::Map::kMaxSpawnSearchRadius * 2) {
      return false;
    }
    if (visited[static_cast<std::size_t>(local_x)]
                 [static_cast<std::size_t>(local_y)]) {
      return false;
    }
    visited[static_cast<std::size_t>(local_x)]
           [static_cast<std::size_t>(local_y)] = true;
    return true;
  };

  if (!mark_visited(start_tx, start_ty)) {
    return std::nullopt;
  }
  queue.push({start_tx, start_ty});

  while (!queue.empty()) {
    const auto [tx, ty] = queue.front();
    queue.pop();

    if (IsValidSpawnTile(map, tx, ty)) {
      return GetTileCenter(tx, ty, level);
    }

    for (const auto [dx, dy] : kCardinalNeighbors) {
      const int next_x = tx + dx;
      const int next_y = ty + dy;
      if (std::abs(next_x - origin_x) > Config::Map::kMaxSpawnSearchRadius ||
          std::abs(next_y - origin_y) > Config::Map::kMaxSpawnSearchRadius) {
        continue;
      }
      if (!mark_visited(next_x, next_y)) {
        continue;
      }
      queue.push({next_x, next_y});
    }
  }

  return std::nullopt;
}

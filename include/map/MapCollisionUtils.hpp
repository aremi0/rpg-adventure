#pragma once

#include <optional>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "components/Components.hpp"
#include "map/LdtkTypes.hpp"

class MapManager;

[[nodiscard]] int GetTileSize(const LdtkLevel& level);

[[nodiscard]] sf::FloatRect GetFootAabb(const TransformComponent& transform,
                                        const BoxColliderComponent& collider);

void WorldToTile(float world_x, float world_y, const LdtkLevel& level, int& tx,
                 int& ty);

[[nodiscard]] sf::Vector2f GetTileCenter(int tx, int ty, const LdtkLevel& level);

[[nodiscard]] bool IsPassableCollision(CollisionType type);

[[nodiscard]] bool IsValidSpawnTile(const MapManager& map, int tx, int ty);

[[nodiscard]] std::optional<sf::Vector2f> FindNearbySpawnPosition(
    const MapManager& map, float world_x, float world_y);


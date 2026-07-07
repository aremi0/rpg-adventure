#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <SFML/System/Vector2.hpp>

#include "components/Components.hpp"
#include "core/Constants.hpp"
#include "map/LdtkTypes.hpp"

const LdtkEntity* FindEntityByIdentifier(const LdtkLevel& level,
                                         std::string_view identifier);

std::optional<std::string> GetFieldString(const LdtkEntity& entity,
                                          std::string_view field_name);

Facing ParseFacing(std::string_view value);

[[nodiscard]] inline constexpr const char* FacingToString(const Facing facing) {
  switch (facing) {
    case Facing::kNorth:
      return "North";
    case Facing::kEast:
      return "East";
    case Facing::kSouth:
      return "South";
    case Facing::kWest:
      return "West";
  }
  return "Unknown";
}

[[nodiscard]] sf::Vector2f GetEntityCenter(const LdtkEntity& entity,
                                             int tile_size);

[[nodiscard]] inline constexpr const char* CollisionTypeToString(
    const CollisionType type) {
  switch (type) {
    case CollisionType::kWalkable:
      return "Walkable";
    case CollisionType::kBlocked:
      return "Blocked";
    case CollisionType::kWater:
      return "Water";
    case CollisionType::kStairs:
      return "Stairs";
  }
  return "Unknown";
}

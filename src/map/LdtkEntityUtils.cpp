#include "map/LdtkEntityUtils.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>

#include "utils/Logger.hpp"

const LdtkEntity* FindEntityByIdentifier(const LdtkLevel& level,
                                         const std::string_view identifier) {
  const auto it = std::ranges::find_if(
      level.entities, [identifier](const LdtkEntity& entity) {
        return entity.identifier == identifier;
      });
  return it != level.entities.end() ? &(*it) : nullptr;
}

std::optional<std::string> GetFieldString(const LdtkEntity& entity,
                                          const std::string_view field_name) {
  if (!entity.field_instances.is_array()) {
    return std::nullopt;
  }

  for (const auto& field : entity.field_instances) {
    if (!field.contains("__identifier") || !field.contains("__value")) {
      continue;
    }
    if (field.at("__identifier").get<std::string>() == field_name) {
      return field.at("__value").get<std::string>();
    }
  }
  return std::nullopt;
}

Facing ParseFacing(const std::string_view value) {
  if (value == "North") {
    return Facing::kNorth;
  }
  if (value == "East") {
    return Facing::kEast;
  }
  if (value == "South") {
    return Facing::kSouth;
  }
  if (value == "West") {
    return Facing::kWest;
  }
  Logger::Warn("Facing LDtk sconosciuto: '{}', uso South", value);
  return Facing::kSouth;
}

sf::Vector2f GetEntityCenter(const LdtkEntity& entity, const int tile_size) {
  const float half = static_cast<float>(tile_size) * 0.5f;
  return {entity.px_x + half, entity.px_y + half};
}

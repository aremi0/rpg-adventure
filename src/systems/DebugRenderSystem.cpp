#include "systems/DebugRenderSystem.hpp"

#include <cmath>
#include <format>
#include <string>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "components/Components.hpp"
#include "core/Constants.hpp"
#include "map/LdtkEntityUtils.hpp"
#include "map/MapCollisionUtils.hpp"
#include "map/MapManager.hpp"

namespace {

sf::Vector2f FacingToVector(Facing facing) {
  switch (facing) {
    case Facing::kNorth:
      return {0.f, -1.f};
    case Facing::kEast:
      return {1.f, 0.f};
    case Facing::kSouth:
      return {0.f, 1.f};
    case Facing::kWest:
      return {-1.f, 0.f};
  }
  return {0.f, 1.f};
}

void DrawFacingArrow(sf::RenderTarget& target, const sf::Vector2f& origin,
                     Facing facing) {
  const sf::Vector2f dir = FacingToVector(facing);
  const float length = 20.f;

  sf::VertexArray arrow(sf::Lines, 2);
  arrow[0].position = origin;
  arrow[0].color = sf::Color::Cyan;
  arrow[1].position = origin + dir * length;
  arrow[1].color = sf::Color::Cyan;
  target.draw(arrow);

  sf::CircleShape tip(3.f);
  tip.setOrigin(3.f, 3.f);
  tip.setPosition(origin + dir * length);
  tip.setFillColor(sf::Color::Cyan);
  target.draw(tip);
}

void DrawTileUnderFeet(sf::RenderTarget& target, int tx, int ty,
                       const LdtkLevel& level) {
  const int tile_size =
      level.tile_size > 0 ? level.tile_size : Config::Map::kTileSize;
  sf::RectangleShape tile_outline(
      sf::Vector2f(static_cast<float>(tile_size), static_cast<float>(tile_size)));
  tile_outline.setPosition(static_cast<float>(level.world_x + tx * tile_size),
                           static_cast<float>(level.world_y + ty * tile_size));
  tile_outline.setFillColor(sf::Color::Transparent);
  tile_outline.setOutlineColor(sf::Color(100, 200, 255, 180));
  tile_outline.setOutlineThickness(1.f);
  target.draw(tile_outline);
}

}  // namespace

void DebugRenderSystem::Draw(entt::registry& registry, sf::RenderTarget& target,
                             const sf::Font& font, const MapManager* map) {
  auto view = registry.view<TransformComponent>();

  for (const auto entity : view) {
    const auto& transform = view.get<TransformComponent>(entity);
    const bool is_player = registry.any_of<PlayerComponent>(entity);

    sf::RectangleShape rect;
    sf::Vector2f size(64.f, 64.f);
    sf::Vector2f rect_position = transform.position;
    sf::Color outline_color = is_player ? sf::Color::Green : sf::Color::Red;

    if (const auto* collider = registry.try_get<BoxColliderComponent>(entity)) {
      size = collider->size;
      rect_position = transform.position + collider->offset;
    } else if (registry.any_of<SpriteComponent>(entity)) {
      const auto& sprite_comp = registry.get<SpriteComponent>(entity);
      const sf::FloatRect bounds = sprite_comp.sprite.getLocalBounds();
      size = sf::Vector2f(bounds.width * transform.scale.x,
                          bounds.height * transform.scale.y);
    }

    rect.setSize(size);
    rect.setOrigin(size.x / 2.f, size.y / 2.f);
    rect.setPosition(rect_position);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(outline_color);
    rect.setOutlineThickness(is_player ? 2.f : 1.f);
    target.draw(rect);

    if (is_player && registry.any_of<SpriteComponent>(entity)) {
      const auto& sprite_comp = registry.get<SpriteComponent>(entity);
      const sf::FloatRect bounds = sprite_comp.sprite.getLocalBounds();
      sf::RectangleShape body_outline(
          sf::Vector2f(bounds.width * transform.scale.x,
                       bounds.height * transform.scale.y));
      body_outline.setOrigin(bounds.width * transform.scale.x * 0.5f,
                             bounds.height * transform.scale.y * 0.5f);
      body_outline.setPosition(transform.position);
      body_outline.setFillColor(sf::Color::Transparent);
      body_outline.setOutlineColor(sf::Color(255, 255, 255, 120));
      body_outline.setOutlineThickness(1.f);
      target.draw(body_outline);
    }

    std::string info_str =
        std::format("ID: {}\nPos: {:.1f}, {:.1f}", static_cast<uint32_t>(entity),
                    transform.position.x, transform.position.y);

    if (const auto* elevation = registry.try_get<ElevationComponent>(entity)) {
      info_str += std::format("\nFloor: {}\nHeight: {:.1f}",
                              elevation->floor_level, elevation->height);
    }

    if (const auto* facing_comp = registry.try_get<FacingComponent>(entity)) {
      info_str +=
          std::format("\nFacing: {}", FacingToString(facing_comp->facing));
    }

    if (const auto* vel = registry.try_get<VelocityComponent>(entity)) {
      info_str +=
          std::format("\nVel: {:.1f}, {:.1f}", vel->velocity.x, vel->velocity.y);
      const float speed = std::sqrt(vel->velocity.x * vel->velocity.x +
                                    vel->velocity.y * vel->velocity.y);
      info_str += std::format("\nSpeed: {:.1f}", speed);
    }

    int tile_x = 0;
    int tile_y = 0;
    if (map != nullptr && registry.any_of<BoxColliderComponent>(entity)) {
      const auto& collider = registry.get<BoxColliderComponent>(entity);
      const sf::FloatRect foot = GetFootAabb(transform, collider);
      const float foot_x = foot.left + foot.width * 0.5f;
      const float foot_y = foot.top + foot.height * 0.5f;
      const auto& level = map->GetLevel();
      WorldToTile(foot_x, foot_y, level, tile_x, tile_y);

      const CollisionType collision = map->CollisionAt(tile_x, tile_y);
      const int tile_elevation = map->ElevationAt(tile_x, tile_y);
      info_str += std::format("\nTile: ({}, {})\nColl: {}\nTileElev: {}",
                              tile_x, tile_y, CollisionTypeToString(collision),
                              tile_elevation);

      if (is_player) {
        DrawTileUnderFeet(target, tile_x, tile_y, level);
      }
    }

    if (const auto* facing_comp = registry.try_get<FacingComponent>(entity)) {
      DrawFacingArrow(target, transform.position, facing_comp->facing);
    }

    sf::Text debug_info;
    debug_info.setFont(font);
    debug_info.setCharacterSize(14);
    debug_info.setFillColor(is_player ? sf::Color::Yellow : sf::Color(200, 200, 200));
    debug_info.setString(info_str);
    debug_info.setPosition(rect_position.x - size.x / 2.f,
                           rect_position.y + size.y / 2.f + 4.f);
    target.draw(debug_info);
  }
}

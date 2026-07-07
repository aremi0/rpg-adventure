#include "map/MapManager.hpp"

#include <string_view>

#include "map/LdtkLoader.hpp"
#include "map/LdtkEntityUtils.hpp"
#include "utils/Logger.hpp"

namespace {

const LdtkIntGridLayer* FindIntGridLayer(const LdtkLevel& level,
                                         std::string_view name) {
  for (const auto& layer : level.int_grid_layers) {
    if (layer.identifier == name) {
      return &layer;
    }
  }
  return nullptr;
}

const LdtkIntGridLayer* FindCollisionLayer(const LdtkLevel& level) {
  if (const auto* layer = FindIntGridLayer(level, "Collision")) {
    return layer;
  }
  return FindIntGridLayer(level, "Collisions");
}

const LdtkTilesetDef* FindWorldTileset(const LdtkLevel& level) {
  for (const auto& tileset : level.tilesets) {
    if (!tileset.rel_path.empty() &&
        tileset.rel_path.starts_with("tilesets/")) {
      return &tileset;
    }
  }
  return nullptr;
}

int GetGridValue(const LdtkIntGridLayer* layer, int tx, int ty) {
  if (layer == nullptr) {
    return 0;
  }

  if (tx < 0 || ty < 0 || tx >= layer->width || ty >= layer->height) {
    return 0;
  }

  const std::size_t index =
      static_cast<std::size_t>(ty * layer->width + tx);
  return layer->values.at(index);
}

}  // namespace

std::expected<void, std::string> MapManager::Load(
    const std::filesystem::path& path) {
  auto level_result = LdtkLoader::LoadFromFile(path);
  if (!level_result) {
    return std::unexpected(level_result.error());
  }

  level_ = std::move(*level_result);
  collisions_ = FindCollisionLayer(level_);
  elevation_ = FindIntGridLayer(level_, "Elevation");

  if (collisions_ == nullptr) {
    return std::unexpected(
        "Layer Collision/Collisions non trovato dopo il parsing LDtk");
  }

  if (elevation_ == nullptr) {
    return std::unexpected("Layer Elevation non trovato dopo il parsing LDtk");
  }

  const auto* world_tileset = FindWorldTileset(level_);
  if (world_tileset == nullptr) {
    return std::unexpected(
        "Nessun tileset world (tilesets/*.png) trovato nel progetto LDtk");
  }

  auto tileset_result = Tileset::Load(*world_tileset, level_.source_path);
  if (!tileset_result) {
    return std::unexpected(tileset_result.error());
  }
  tileset_ = std::move(*tileset_result);

  Logger::Info("MapManager pronto: {} tile layers, {} int grids, {} entities",
               level_.tile_layers.size(), level_.int_grid_layers.size(),
               level_.entities.size());
  return {};
}

const LdtkLevel& MapManager::GetLevel() const { return level_; }

const Tileset& MapManager::GetTileset() const { return tileset_; }

bool MapManager::HasTileset() const { return tileset_.IsLoaded(); }

const LdtkEntity* MapManager::FindEntity(const std::string_view identifier) const {
  return FindEntityByIdentifier(level_, identifier);
}

CollisionType MapManager::CollisionAt(int tx, int ty) const {
  if (collisions_ == nullptr) {
    return CollisionType::kBlocked;
  }

  if (tx < 0 || ty < 0 || tx >= collisions_->width ||
      ty >= collisions_->height) {
    return CollisionType::kBlocked;
  }

  return NormalizeCollision(GetGridValue(collisions_, tx, ty));
}

int MapManager::ElevationAt(int tx, int ty) const {
  if (elevation_ == nullptr) {
    return 0;
  }

  if (tx < 0 || ty < 0 || tx >= elevation_->width ||
      ty >= elevation_->height) {
    return 0;
  }

  return NormalizeElevation(GetGridValue(elevation_, tx, ty));
}

const LdtkIntGridLayer* MapManager::GetIntGridLayer(
    std::string_view name) const {
  return FindIntGridLayer(level_, name);
}

CollisionType MapManager::NormalizeCollision(int raw) {
  switch (raw) {
    case 0:
    case 1:
      return CollisionType::kWalkable;
    case 2:
      return CollisionType::kBlocked;
    case 3:
      return CollisionType::kWater;
    case 4:
      return CollisionType::kStairs;
    default:
      Logger::Warn("Valore collisione LDtk sconosciuto: {}", raw);
      return CollisionType::kBlocked;
  }
}

int MapManager::NormalizeElevation(int raw) {
  switch (raw) {
    case 0:
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 2;
    default:
      Logger::Warn("Valore elevazione LDtk sconosciuto: {}", raw);
      return 0;
  }
}

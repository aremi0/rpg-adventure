#include "map/LdtkLoader.hpp"

#include <fstream>
#include <format>
#include <span>
#include <string_view>

#include <nlohmann/json.hpp>

#include "core/Constants.hpp"
#include "map/MapLayerNames.hpp"
#include "utils/Logger.hpp"

namespace {

bool ContainsName(std::string_view identifier,
                  std::span<const std::string_view> names) {
  for (const auto name : names) {
    if (identifier == name) {
      return true;
    }
  }
  return false;
}

bool IsKnownLayer(std::string_view identifier, std::string_view type) {
  if (type == "Tiles") {
    return ContainsName(identifier, MapLayerNames::kTileLayers);
  }
  if (type == "IntGrid") {
    return ContainsName(identifier, MapLayerNames::kIntGridLayers);
  }
  if (type == "Entities") {
    return ContainsName(identifier, MapLayerNames::kEntityLayers);
  }
  return false;
}

LdtkTilesetDef ParseTilesetDef(const nlohmann::json& tileset_json) {
  LdtkTilesetDef def;
  def.uid = tileset_json.at("uid").get<std::uint32_t>();
  def.identifier = tileset_json.at("identifier").get<std::string>();
  def.rel_path = tileset_json.at("relPath").get<std::string>();
  def.tile_grid_size = tileset_json.at("tileGridSize").get<int>();
  def.columns = tileset_json.at("__cWid").get<int>();
  def.rows = tileset_json.at("__cHei").get<int>();
  return def;
}

std::vector<LdtkTilesetDef> ParseTilesets(const nlohmann::json& root) {
  std::vector<LdtkTilesetDef> tilesets;
  if (!root.contains("defs") || !root.at("defs").contains("tilesets")) {
    return tilesets;
  }

  for (const auto& tileset_json : root.at("defs").at("tilesets")) {
    tilesets.push_back(ParseTilesetDef(tileset_json));
  }
  return tilesets;
}

LdtkTileLayer ParseTileLayer(const nlohmann::json& layer_json) {
  LdtkTileLayer layer;
  layer.identifier = layer_json.at("__identifier").get<std::string>();
  layer.grid_size = layer_json.at("__gridSize").get<int>();
  layer.width = layer_json.at("__cWid").get<int>();
  layer.height = layer_json.at("__cHei").get<int>();
  layer.tile_data.assign(static_cast<std::size_t>(layer.width * layer.height),
                         0);

  for (const auto& tile_json : layer_json.at("gridTiles")) {
    const int dest_index = tile_json.at("d").at(0).get<int>();
    const std::uint32_t tile_id = tile_json.at("t").get<std::uint32_t>();
    if (dest_index >= 0 &&
        dest_index < static_cast<int>(layer.tile_data.size())) {
      layer.tile_data[static_cast<std::size_t>(dest_index)] = tile_id;
    }
  }

  return layer;
}

LdtkIntGridLayer ParseIntGridLayer(const nlohmann::json& layer_json) {
  LdtkIntGridLayer layer;
  layer.identifier = layer_json.at("__identifier").get<std::string>();
  layer.width = layer_json.at("__cWid").get<int>();
  layer.height = layer_json.at("__cHei").get<int>();
  layer.values = layer_json.at("intGridCsv").get<std::vector<int>>();
  return layer;
}

std::vector<LdtkEntity> ParseEntities(const nlohmann::json& layer_json) {
  std::vector<LdtkEntity> entities;

  for (const auto& entity_json : layer_json.at("entityInstances")) {
    LdtkEntity entity;
    entity.identifier = entity_json.at("__identifier").get<std::string>();
    entity.px_x = static_cast<float>(entity_json.at("px").at(0).get<int>());
    entity.px_y = static_cast<float>(entity_json.at("px").at(1).get<int>());
    entity.grid_x = entity_json.at("__grid").at(0).get<int>();
    entity.grid_y = entity_json.at("__grid").at(1).get<int>();
    entity.field_instances = entity_json.at("fieldInstances");
    entities.push_back(std::move(entity));
  }

  return entities;
}

bool HasIntGridLayer(const LdtkLevel& level, std::string_view name) {
  for (const auto& layer : level.int_grid_layers) {
    if (layer.identifier == name) {
      return true;
    }
  }
  return false;
}

}  // namespace

std::expected<LdtkLevel, std::string> LdtkLoader::LoadFromFile(
    const std::filesystem::path& path) {
  std::ifstream file{path};
  if (!file.is_open()) {
    return std::unexpected(
        std::format("Impossibile aprire il file LDtk: {}", path.string()));
  }

  nlohmann::json root;
  try {
    file >> root;
  } catch (const nlohmann::json::exception& ex) {
    return std::unexpected(
        std::format("JSON LDtk non valido in {}: {}", path.string(), ex.what()));
  }

  if (!root.contains("levels") || root.at("levels").empty()) {
    return std::unexpected(
        std::format("Nessun livello trovato in {}", path.string()));
  }

  const auto& level_json = root.at("levels").at(0);
  LdtkLevel level;
  level.source_path = path;
  level.identifier = level_json.at("identifier").get<std::string>();
  level.world_x = level_json.at("worldX").get<int>();
  level.world_y = level_json.at("worldY").get<int>();
  level.px_width = level_json.at("pxWid").get<int>();
  level.px_height = level_json.at("pxHei").get<int>();
  level.tilesets = ParseTilesets(root);

  if (root.contains("defaultGridSize")) {
    level.tile_size = root.at("defaultGridSize").get<int>();
    if (level.tile_size != Config::Map::kTileSize) {
      Logger::Warn("LDtk tile_size inatteso: {} (atteso {})", level.tile_size,
                   Config::Map::kTileSize);
    }
  }

  for (const auto& layer_json : level_json.at("layerInstances")) {
    const std::string identifier =
        layer_json.at("__identifier").get<std::string>();
    const std::string type = layer_json.at("__type").get<std::string>();

    if (!IsKnownLayer(identifier, type)) {
      Logger::Debug("Layer LDtk ignorato: {} ({})", identifier, type);
      continue;
    }

    if (type == "Tiles") {
      level.tile_layers.push_back(ParseTileLayer(layer_json));
      continue;
    }

    if (type == "IntGrid") {
      level.int_grid_layers.push_back(ParseIntGridLayer(layer_json));
      continue;
    }

    if (type == "Entities") {
      const auto entities = ParseEntities(layer_json);
      level.entities.insert(level.entities.end(), entities.begin(),
                            entities.end());
    }
  }

  if (!level.tile_layers.empty()) {
    level.grid_width = level.tile_layers.front().width;
    level.grid_height = level.tile_layers.front().height;
  } else if (!level.int_grid_layers.empty()) {
    level.grid_width = level.int_grid_layers.front().width;
    level.grid_height = level.int_grid_layers.front().height;
  }

  const bool has_collision =
      HasIntGridLayer(level, "Collision") ||
      HasIntGridLayer(level, "Collisions");
  if (!has_collision) {
    return std::unexpected(
        "Layer IntGrid Collision/Collisions mancante nel livello LDtk");
  }

  if (!HasIntGridLayer(level, "Elevation")) {
    return std::unexpected("Layer IntGrid Elevation mancante nel livello LDtk");
  }

  Logger::Info(
      "LDtk livello '{}' caricato: {}x{} tile, {} tileset",
      level.identifier, level.grid_width, level.grid_height,
      level.tilesets.size());
  return level;
}

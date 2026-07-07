#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/Constants.hpp"

enum class CollisionType : std::uint8_t {
  kWalkable = 0,
  kBlocked = 1,
  kWater = 2,
  kStairs = 3
};

struct LdtkTilesetDef {
  std::uint32_t uid = 0;
  std::string identifier;
  std::string rel_path;
  int tile_grid_size = Config::Map::kTileSize;
  int columns = 0;
  int rows = 0;
};

struct LdtkTileLayer {
  std::string identifier;
  int grid_size = Config::Map::kTileSize;
  int width = 0;
  int height = 0;
  std::vector<std::uint32_t> tile_data;
};

struct LdtkIntGridLayer {
  std::string identifier;
  int width = 0;
  int height = 0;
  std::vector<int> values;
};

struct LdtkEntity {
  std::string identifier;
  float px_x = 0.f;
  float px_y = 0.f;
  int grid_x = 0;
  int grid_y = 0;
  nlohmann::json field_instances;
};

struct LdtkLevel {
  std::string identifier;
  int world_x = 0;
  int world_y = 0;
  int px_width = 0;
  int px_height = 0;
  int grid_width = 0;
  int grid_height = 0;
  int tile_size = Config::Map::kTileSize;
  std::filesystem::path source_path;
  std::vector<LdtkTilesetDef> tilesets;
  std::vector<LdtkTileLayer> tile_layers;
  std::vector<LdtkIntGridLayer> int_grid_layers;
  std::vector<LdtkEntity> entities;
};

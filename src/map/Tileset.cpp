#include "map/Tileset.hpp"

#include <format>

#include "utils/Logger.hpp"

std::expected<Tileset, std::string> Tileset::Load(
    const LdtkTilesetDef& def, const std::filesystem::path& ldtk_file) {
  if (def.rel_path.empty()) {
    return std::unexpected(std::format(
        "Tileset '{}' (uid {}) non ha relPath", def.identifier, def.uid));
  }

  const auto texture_path = ldtk_file.parent_path() / def.rel_path;
  Tileset tileset;
  if (!tileset.texture_.loadFromFile(texture_path.string())) {
    return std::unexpected(std::format(
        "Impossibile caricare tileset '{}' da {}", def.identifier,
        texture_path.string()));
  }

  tileset.tile_grid_size_ = def.tile_grid_size;
  tileset.columns_ = def.columns;

  Logger::Info("Tileset '{}' caricato: {} ({}x{} tile, griglia {}px)",
               def.identifier, texture_path.string(), def.columns, def.rows,
               def.tile_grid_size);
  return tileset;
}

const sf::Texture& Tileset::GetTexture() const { return texture_; }

sf::IntRect Tileset::GetTileRect(const std::uint32_t tile_id) const {
  if (columns_ <= 0 || tile_grid_size_ <= 0) {
    return {};
  }

  const int col = static_cast<int>(tile_id % static_cast<std::uint32_t>(columns_));
  const int row = static_cast<int>(tile_id / static_cast<std::uint32_t>(columns_));
  return {col * tile_grid_size_, row * tile_grid_size_, tile_grid_size_,
          tile_grid_size_};
}

int Tileset::GetTileGridSize() const { return tile_grid_size_; }

bool Tileset::IsLoaded() const { return texture_.getSize().x > 0; }

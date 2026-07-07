#pragma once

#include <expected>
#include <filesystem>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "map/LdtkTypes.hpp"

class Tileset {
 public:
  [[nodiscard]] static std::expected<Tileset, std::string> Load(
      const LdtkTilesetDef& def, const std::filesystem::path& ldtk_file);

  [[nodiscard]] const sf::Texture& GetTexture() const;
  [[nodiscard]] sf::IntRect GetTileRect(std::uint32_t tile_id) const;
  [[nodiscard]] int GetTileGridSize() const;
  [[nodiscard]] bool IsLoaded() const;

 private:
  sf::Texture texture_;
  int tile_grid_size_ = 0;
  int columns_ = 0;
};

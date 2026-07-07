#pragma once

#include <expected>
#include <filesystem>
#include <string_view>

#include "map/LdtkTypes.hpp"
#include "map/Tileset.hpp"

class MapManager {
 public:
  std::expected<void, std::string> Load(const std::filesystem::path& path);

  [[nodiscard]] const LdtkLevel& GetLevel() const;
  [[nodiscard]] CollisionType CollisionAt(int tx, int ty) const;
  [[nodiscard]] int ElevationAt(int tx, int ty) const;
  [[nodiscard]] const LdtkIntGridLayer* GetIntGridLayer(
      std::string_view name) const;
  [[nodiscard]] const Tileset& GetTileset() const;
  [[nodiscard]] bool HasTileset() const;
  [[nodiscard]] const LdtkEntity* FindEntity(std::string_view identifier) const;

 private:
  LdtkLevel level_;
  Tileset tileset_;
  const LdtkIntGridLayer* collisions_ = nullptr;
  const LdtkIntGridLayer* elevation_ = nullptr;

  static CollisionType NormalizeCollision(int raw);
  static int NormalizeElevation(int raw);
};

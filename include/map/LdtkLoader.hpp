#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "map/LdtkTypes.hpp"

class LdtkLoader {
 public:
  [[nodiscard]] static std::expected<LdtkLevel, std::string> LoadFromFile(
      const std::filesystem::path& path);
};

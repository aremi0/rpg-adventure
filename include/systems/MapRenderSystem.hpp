#pragma once

#include <expected>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "map/LdtkTypes.hpp"
#include "map/Tileset.hpp"

class MapRenderSystem {
 public:
  [[nodiscard]] std::expected<void, std::string> Build(
      const LdtkLevel& level, const Tileset& tileset);
  void Draw(sf::RenderTarget& target) const;
  void Clear();

 private:
  std::vector<sf::VertexArray> layer_batches_;
  const sf::Texture* texture_ = nullptr;
};

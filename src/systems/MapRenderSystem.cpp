#include "systems/MapRenderSystem.hpp"

#include <string_view>

#include <SFML/Graphics/Vertex.hpp>

#include "core/Constants.hpp"
#include "map/MapLayerNames.hpp"
#include "utils/Logger.hpp"

namespace {

const LdtkTileLayer* FindTileLayer(const LdtkLevel& level,
                                   std::string_view name) {
  for (const auto& layer : level.tile_layers) {
    if (layer.identifier == name) {
      return &layer;
    }
  }
  return nullptr;
}

void AppendTileQuad(sf::VertexArray& vertices, float x, float y, int tile_size,
                    const sf::IntRect& src_rect) {
  const float tu0 = static_cast<float>(src_rect.left);
  const float tv0 = static_cast<float>(src_rect.top);
  const float tu1 = tu0 + static_cast<float>(src_rect.width);
  const float tv1 = tv0 + static_cast<float>(src_rect.height);
  const float x1 = x + static_cast<float>(tile_size);
  const float y1 = y + static_cast<float>(tile_size);

  vertices.append(sf::Vertex{{x, y}, {tu0, tv0}});
  vertices.append(sf::Vertex{{x1, y}, {tu1, tv0}});
  vertices.append(sf::Vertex{{x1, y1}, {tu1, tv1}});
  vertices.append(sf::Vertex{{x, y1}, {tu0, tv1}});
}

sf::VertexArray BuildLayerVertices(const LdtkTileLayer& layer,
                                   const Tileset& tileset,
                                   const LdtkLevel& level) {
  const int tile_size = level.tile_size > 0 ? level.tile_size
                                            : Config::Map::kTileSize;
  const float world_x = static_cast<float>(level.world_x);
  const float world_y = static_cast<float>(level.world_y);

  sf::VertexArray vertices(sf::Quads);

  for (int ty = 0; ty < layer.height; ++ty) {
    for (int tx = 0; tx < layer.width; ++tx) {
      const std::size_t index =
          static_cast<std::size_t>(ty * layer.width + tx);
      const std::uint32_t tile_id = layer.tile_data[index];
      if (tile_id == 0) {
        continue;
      }

      const float px = world_x + static_cast<float>(tx * tile_size);
      const float py = world_y + static_cast<float>(ty * tile_size);
      AppendTileQuad(vertices, px, py, tile_size,
                     tileset.GetTileRect(tile_id));
    }
  }

  return vertices;
}

}  // namespace

std::expected<void, std::string> MapRenderSystem::Build(
    const LdtkLevel& level, const Tileset& tileset) {
  if (!tileset.IsLoaded()) {
    return std::unexpected("Tileset non caricato per MapRenderSystem::Build");
  }

  Clear();
  texture_ = &tileset.GetTexture();

  for (const auto layer_name : MapLayerNames::kTileRenderOrder) {
    const auto* layer = FindTileLayer(level, layer_name);
    if (layer == nullptr) {
      Logger::Debug("MapRenderSystem: layer '{}' assente, skip", layer_name);
      continue;
    }

    auto vertices = BuildLayerVertices(*layer, tileset, level);
    if (vertices.getVertexCount() == 0) {
      Logger::Debug("MapRenderSystem: layer '{}' vuoto, skip", layer_name);
      continue;
    }

    Logger::Debug("MapRenderSystem: layer '{}' -> {} quad",
                  layer_name, vertices.getVertexCount() / 4);
    layer_batches_.push_back(std::move(vertices));
  }

  if (layer_batches_.empty()) {
    return std::unexpected(
        "Nessun tile layer renderizzabile trovato nella mappa LDtk");
  }

  Logger::Info("MapRenderSystem pronto: {} layer batch", layer_batches_.size());
  return {};
}

void MapRenderSystem::Draw(sf::RenderTarget& target) const {
  if (texture_ == nullptr) {
    return;
  }

  const sf::RenderStates states(texture_);
  for (const auto& batch : layer_batches_) {
    target.draw(batch, states);
  }
}

void MapRenderSystem::Clear() {
  layer_batches_.clear();
  texture_ = nullptr;
}

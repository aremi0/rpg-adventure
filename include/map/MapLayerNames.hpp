#pragma once

#include <string_view>

namespace MapLayerNames {

constexpr std::string_view kTileLayers[] = {"Terrain", "Decor", "Buildings"};
constexpr std::string_view kIntGridLayers[] = {"Collision", "Collisions",
                                               "Elevation"};
constexpr std::string_view kEntityLayers[] = {"Entities"};

// Ordine di disegno bottom → top (condiviso tra loader e MapRenderSystem).
constexpr std::string_view kTileRenderOrder[] = {"Terrain", "Decor", "Buildings"};

}  // namespace MapLayerNames

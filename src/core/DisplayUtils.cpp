#include "core/DisplayUtils.hpp"

#include "core/Constants.hpp"

void ApplyDisplayView(sf::RenderWindow& window) {
  sf::View logical_view(sf::FloatRect(
      0.f, 0.f, static_cast<float>(Config::Game::kLogicalWidth),
      static_cast<float>(Config::Game::kLogicalHeight)));
  window.setView(logical_view);
}

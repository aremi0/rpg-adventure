#include "systems/PlayerInputSystem.hpp"
#include "components/Components.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

void PlayerInputSystem::Update(entt::registry& registry) {
    // Cerchiamo l'entità che ha sia il tag Player sia una Velocità
    auto view = registry.view<PlayerComponent, VelocityComponent>();

    for (auto entity : view) {
        auto& vel = view.get<VelocityComponent>(entity);

        sf::Vector2f direction{};

        // Lettura di input (Supporta sia WASD che Frecce Direzionali)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            direction.y -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            direction.y += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            direction.x -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            direction.x += 1.0f;

        // Normalizzazione: Se premi contemporaneamente W e D, il vettore sarebbe {1, -1}.
        // La sua lunghezza (magnitudo) sarebbe sqrt(2) (~1.41).
        // Dividendo per la lunghezza, rendiamo la velocità diagonale uguale a quella laterale.
        if (direction.x != 0.0f || direction.y != 0.0f) {
            float length = std::hypot(direction.x, direction.y);
            direction /= length; // Ora la lunghezza è 1.0f
        }

        // Applichiamo la direzione alla velocità (Direzione * Velocità base)
        vel.velocity = direction * vel.speed;
    }
}
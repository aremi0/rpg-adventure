#include "systems/MovementSystem.hpp"
#include "components/Components.hpp"

void MovementSystem::Update(entt::registry& registry, float deltaTime) {
    // Cerchiamo tutte le entità che hanno Posizione e Velocità
    auto view = registry.view<TransformComponent, VelocityComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        const auto& vel = view.get<VelocityComponent>(entity);

        // Spostamento fluido a virgola mobile
        // Nuova Posizione = Vecchia Posizione + (Velocità * Tempo Trascorso)
        transform.position += vel.velocity * deltaTime;
    }
}
#pragma once
#include <entt/entt.hpp>

// Sostituito da CollisionSystem in Fase 7 (movimento + collisioni griglia).
// Mantenuto per riferimento storico; non usato in GameState.
class MovementSystem {
    public:
        // Qui serve il dt per calcolare lo spostamento fluido
        static void Update(entt::registry& registry, float deltaTime);
};
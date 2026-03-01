#pragma once
#include <entt/entt.hpp>

class MovementSystem {
    public:
        // Qui serve il dt per calcolare lo spostamento fluido
        static void Update(entt::registry& registry, float deltaTime);
};
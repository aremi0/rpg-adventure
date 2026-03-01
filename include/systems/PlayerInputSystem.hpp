#pragma once
#include <entt/entt.hpp>

class PlayerInputSystem {
    public:
        static void Update(entt::registry& registry); // Non serve il deltaTime qui, stiamo solo registrando l'intento
};
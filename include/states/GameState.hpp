#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"
#include <entt/entt.hpp>

class GameState : public State {
    public:
        GameState(GameDataRef data);

        void Init() override;
        void HandleInput() override;
        void Update(float dt) override;
        void UpdateVisuals(float dt) override;
        void Draw() override;

        // Metodi per gestire l'overlay
        //void Pause() override;
        //void Resume() override;

    private:
        GameDataRef data_;
        bool is_paused_ = false;

        entt::entity hero_;
};
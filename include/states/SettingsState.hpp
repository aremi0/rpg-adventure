#pragma once
#include <memory>
#include <vector>
#include "core/State.hpp"
#include "core/Game.hpp"
#include "ui/Button.hpp"

class SettingsState : public State {
    public:
        SettingsState(GameDataRef data);

        void Init() override;
        void HandleInput() override;
        void Update(float dt) override;
        void Draw() override;

    private:
        GameDataRef data_;
            
        // Usiamo unique_ptr per gestire in automatico la memoria dei bottoni
        std::unique_ptr<Button> res_button_;
        std::unique_ptr<Button> vol_button_;
        std::unique_ptr<Button> back_button_;

        sf::Sprite background_sprite_;

        // Variabili per tenere traccia dello stato delle opzioni
        int volume_level_;
        int res_index_;
        std::vector<sf::VideoMode> supported_resolutions_;

        // Helper per aggiornare il testo dei bottoni
        void CycleVolume();
        void CycleResolution();
};
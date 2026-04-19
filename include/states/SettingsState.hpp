#pragma once
#include <memory>
#include <vector>
#include "core/State.hpp"
#include "core/Game.hpp"
#include "ui/Button.hpp"

// Raggruppa una riga di impostazione: [◀ freccia] [label] [▶ freccia]
struct SettingRow {
    std::unique_ptr<Button> left;   // Freccia sinistra (interattiva)
    std::unique_ptr<Button> label;  // Label centrale (non interattiva)
    std::unique_ptr<Button> right;  // Freccia destra (interattiva)

    void Update(const sf::Vector2f& mouse_pos);
    void Render(sf::RenderTarget& target);
    void SetVolume(float volume); // Aggiorna volume solo delle frecce
};

class SettingsState : public State {
    public:
        SettingsState(GameDataRef data);

        void Init() override;
        void HandleInput() override;
        void Update(float dt) override;
        void Draw() override;

    private:
        GameDataRef data_;
            
        // Righe di impostazione (frecce + label)
        SettingRow master_row_;
        SettingRow music_row_;
        SettingRow sfx_row_;
        SettingRow ui_row_;
        SettingRow res_row_;

        std::unique_ptr<Button> back_button_;

        sf::Sprite background_sprite_;

        // Copia di lavoro delle impostazioni (modificate in RAM, salvate solo su "Indietro")
        UserSettings temp_settings_;

        // Risoluzione
        int selected_res_index_;  // Indice sfogliato con frecce (navigazione)
        int applied_res_index_;   // Indice applicato alla finestra
        std::vector<sf::VideoMode> supported_resolutions_;

        // Helper
        SettingRow MakeSettingRow(float y, const std::string& label_text,
                                 const sf::Font& font,
                                 const sf::SoundBuffer* hover_sfx,
                                 const sf::SoundBuffer* click_sfx,
                                 float ui_vol);
        void AdjustVolume(int& channel, int delta, const std::string_view& name, Button& label);
        void PrevResolution();    // Sfoglia alla risoluzione precedente
        void NextResolution();    // Sfoglia alla risoluzione successiva
        void ApplyResolution();   // Applica la risoluzione selezionata (RAM + finestra)
        void UpdateResLabel();    // Aggiorna testo/colore della label risoluzione
        void ApplyVolumes();      // Applica volumi (feedback immediato in RAM)
        void SaveToConfig();      // Conferma: salva temp_settings_ nel ConfigManager e su disco

        // Trova l'indice della risoluzione nella lista supportata
        [[nodiscard]] int FindResolutionIndex(sf::Vector2u resolution) const;
};
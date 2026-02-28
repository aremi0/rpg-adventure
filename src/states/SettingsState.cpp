#include "states/SettingsState.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <format>
#include <SFML/Audio.hpp>
#include <algorithm>

// ==================== SettingRow ====================

void SettingRow::Update(const sf::Vector2f& mouse_pos) {
    left->Update(mouse_pos);
    // label non viene aggiornata (non interattiva)
    right->Update(mouse_pos);
}

void SettingRow::Render(sf::RenderTarget& target) {
    left->Render(target);
    label->Render(target);
    right->Render(target);
}

void SettingRow::SetVolume(float volume) {
    left->SetVolume(volume);
    // label non ha suoni
    right->SetVolume(volume);
}

// ==================== SettingsState ====================

SettingsState::SettingsState(GameDataRef data)
    : State("SettingsState"), data_(data),
      selected_res_index_(data->resolution_index) { // Legge l'indice persistente da GameData
        supported_resolutions_ = {
            sf::VideoMode(800, 600),
            sf::VideoMode(Config::Game::kWindowWidth, Config::Game::kWindowHeight),
            sf::VideoMode(1280, 720)
        };
    }

SettingRow SettingsState::MakeSettingRow(
    float y, const std::string& label_text,
    const sf::Font& font,
    const sf::SoundBuffer* hover_sfx, const sf::SoundBuffer* click_sfx,
    float ui_vol)
{
    // Layout: centro della risoluzione logica, larghezza totale ~340px
    // [◀ 40px] [gap 5px] [label 250px] [gap 5px] [▶ 40px]
    float center_x = Config::Game::kWindowWidth / 2.0f;
    constexpr float arrow_w = 40.0f;
    constexpr float label_w = 250.0f;
    constexpr float gap = 5.0f;
    constexpr float row_h = 50.0f;
    constexpr float total_w = arrow_w + gap + label_w + gap + arrow_w;

    float start_x = center_x - total_w / 2.0f;

    SettingRow row;

    // Freccia sinistra (interattiva, con suoni)
    row.left = std::make_unique<Button>(
        start_x, y, arrow_w, row_h, font,
        std::string(Config::Settings::kArrowLeft), 24,
        Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol,
        hover_sfx, click_sfx, ui_vol
    );

    // Label centrale (NON interattiva: stessi colori per tutti gli stati, nessun suono)
    sf::Color label_col(Config::GUI::kLabelCol);
    row.label = std::make_unique<Button>(
        start_x + arrow_w + gap, y, label_w, row_h, font,
        label_text, 22,
        label_col, label_col, label_col,  // Idle = Hover = Active → nessuna animazione
        nullptr, nullptr                   // Nessun suono
    );

    // Freccia destra (interattiva, con suoni)
    row.right = std::make_unique<Button>(
        start_x + arrow_w + gap + label_w + gap, y, arrow_w, row_h, font,
        std::string(Config::Settings::kArrowRight), 24,
        Config::GUI::kIdleCol, Config::GUI::kHoverCol, Config::GUI::kActiveCol,
        hover_sfx, click_sfx, ui_vol
    );

    return row;
}

void SettingsState::Init() {
    Logger::Debug("Inizializzazione ({})", this->GetStateName());

    // ----------------- Loading Game sounds
    auto hover_res = data_->assets.LoadAsset<sf::SoundBuffer>(
        std::string(Config::Game::kButtonHoverSfxName), 
        std::string(Config::Game::kButtonHoverSfxPath)
    );
    auto click_res = data_->assets.LoadAsset<sf::SoundBuffer>(
        std::string(Config::Game::kButtonClickSfxName), 
        std::string(Config::Game::kButtonClickSfxPath)
    );

    // Puntatori: nullptr se il caricamento è fallito, altrimenti puntano al buffer
    const sf::SoundBuffer* hover_sfx = hover_res 
        ? &data_->assets.GetAsset<sf::SoundBuffer>(std::string(Config::Game::kButtonHoverSfxName)) 
        : nullptr;
    const sf::SoundBuffer* click_sfx = click_res 
        ? &data_->assets.GetAsset<sf::SoundBuffer>(std::string(Config::Game::kButtonClickSfxName)) 
        : nullptr;

    // ----------------- Creating Settings rows
    const sf::Font& font = data_->assets.GetAsset<sf::Font>(std::string(Config::Game::kFontName));
    const float ui_vol = data_->audio.GetUiVolume();

    float start_y = 200.0f;
    float spacing = 65.0f;

    master_row_ = MakeSettingRow(start_y, 
        std::format("{}: {}%", Config::Settings::kMasterVolumeName, data_->audio.master),
        font, hover_sfx, click_sfx, ui_vol);

    music_row_ = MakeSettingRow(start_y + spacing, 
        std::format("{}: {}%", Config::Settings::kMusicVolumeName, data_->audio.music),
        font, hover_sfx, click_sfx, ui_vol);

    sfx_row_ = MakeSettingRow(start_y + spacing * 2, 
        std::format("{}: {}%", Config::Settings::kSfxVolumeName, data_->audio.sfx),
        font, hover_sfx, click_sfx, ui_vol);

    ui_row_ = MakeSettingRow(start_y + spacing * 3, 
        std::format("{}: {}%", Config::Settings::kUiVolumeName, data_->audio.ui),
        font, hover_sfx, click_sfx, ui_vol);

    res_row_ = MakeSettingRow(start_y + spacing * 4,
        std::format("{}: {}x{}", Config::Settings::kRisoluzioneName,
                    supported_resolutions_[selected_res_index_].width,
                    supported_resolutions_[selected_res_index_].height),
        font, hover_sfx, click_sfx, ui_vol);

    // Bottone Indietro (centrato, sotto le righe)
    float center_x = Config::Game::kWindowWidth / 2.0f - 150.0f;
    back_button_ = std::make_unique<Button>(
        center_x, start_y + spacing * 5, 300.0f, 50.0f, font,
        std::string(Config::Settings::kIndietroName), 24,
        Config::GUI::kIdleRedCol, Config::GUI::kHoverRedCol, Config::GUI::kActiveRedCol,
        hover_sfx, click_sfx, ui_vol
    );
}

void SettingsState::HandleInput() {
    sf::Event event;
    while (data_->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            data_->window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            Logger::Debug("Tasto premuto: ({})", (int)event.key.code);
        }
    }
}

void SettingsState::Update(float dt) {
    // Mappiamo le coordinate del mouse rispetto alla finestra
    sf::Vector2f mouse_pos = data_->window.mapPixelToCoords(sf::Mouse::getPosition(data_->window));

    // Aggiorniamo lo stato delle righe e del bottone indietro
    master_row_.Update(mouse_pos);
    music_row_.Update(mouse_pos);
    sfx_row_.Update(mouse_pos);
    ui_row_.Update(mouse_pos);
    res_row_.Update(mouse_pos);
    // Se la risoluzione selezionata è diversa da quella attuale, la label diventa interattiva
    if (selected_res_index_ != data_->resolution_index) {
        res_row_.label->Update(mouse_pos);
    } else {
        res_row_.label->Reset(); // Pulisce lo stato quando inattivo per evitare click intrappolati
    }
    back_button_->Update(mouse_pos);

    // Logica dei click — Volume
    if (master_row_.left->IsPressed())
        AdjustVolume(data_->audio.master, -Config::Settings::kVolumeStep, Config::Settings::kMasterVolumeName, *master_row_.label);
    if (master_row_.right->IsPressed())
        AdjustVolume(data_->audio.master, +Config::Settings::kVolumeStep, Config::Settings::kMasterVolumeName, *master_row_.label);

    if (music_row_.left->IsPressed())
        AdjustVolume(data_->audio.music, -Config::Settings::kVolumeStep, Config::Settings::kMusicVolumeName, *music_row_.label);
    if (music_row_.right->IsPressed())
        AdjustVolume(data_->audio.music, +Config::Settings::kVolumeStep, Config::Settings::kMusicVolumeName, *music_row_.label);

    if (sfx_row_.left->IsPressed())
        AdjustVolume(data_->audio.sfx, -Config::Settings::kVolumeStep, Config::Settings::kSfxVolumeName, *sfx_row_.label);
    if (sfx_row_.right->IsPressed())
        AdjustVolume(data_->audio.sfx, +Config::Settings::kVolumeStep, Config::Settings::kSfxVolumeName, *sfx_row_.label);

    if (ui_row_.left->IsPressed())
        AdjustVolume(data_->audio.ui, -Config::Settings::kVolumeStep, Config::Settings::kUiVolumeName, *ui_row_.label);
    if (ui_row_.right->IsPressed())
        AdjustVolume(data_->audio.ui, +Config::Settings::kVolumeStep, Config::Settings::kUiVolumeName, *ui_row_.label);

    // Logica dei click — Risoluzione
    if (res_row_.left->IsPressed()) {
        PrevResolution();
    }
    if (res_row_.right->IsPressed()) {
        NextResolution();
    }
    // Click sulla label verde = Applica la risoluzione selezionata
    if (selected_res_index_ != data_->resolution_index && res_row_.label->IsPressed()) {
        ApplyResolution();
    }

    // Bottone Indietro
    if (back_button_->IsPressed()) {
        Logger::Trace("Ritorno al menu principale");
        data_->machine.RemoveState(); // Toglie SettingsState e fa il Resume di MainMenuState
    }
}

void SettingsState::Draw() {
    // Usiamo le costanti della Risoluzione Logica invece della grandezza fisica della finestra
    sf::RectangleShape overlay(sf::Vector2f(
        static_cast<float>(Config::Game::kWindowWidth), 
        static_cast<float>(Config::Game::kWindowHeight)
    ));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    data_->window.draw(overlay);

    // Disegniamo le righe di impostazione e il bottone indietro
    master_row_.Render(data_->window);
    music_row_.Render(data_->window);
    sfx_row_.Render(data_->window);
    ui_row_.Render(data_->window);
    res_row_.Render(data_->window);
    back_button_->Render(data_->window);
}

void SettingsState::AdjustVolume(int& channel, int delta, const std::string_view& name, Button& label) {
    channel = std::clamp(channel + delta, 
                         Config::Settings::kMinVolume, 
                         Config::Settings::kMaxVolume);

    Logger::Info("{} impostato a: {}%", name, channel);
    label.SetText(std::format("{}: {}%", name, channel));

    ApplyVolumes();
}

void SettingsState::ApplyVolumes() {
    // Aggiorna volume della musica in riproduzione
    if (data_->assets.HasMusic(std::string(Config::Game::kMusicName))) {
        sf::Music& music = data_->assets.GetMusic(std::string(Config::Game::kMusicName));
        music.setVolume(data_->audio.GetMusicVolume());
    }

    // Aggiorna volume dei suoni UI su tutte le frecce
    float ui_vol = data_->audio.GetUiVolume();
    master_row_.SetVolume(ui_vol);
    music_row_.SetVolume(ui_vol);
    sfx_row_.SetVolume(ui_vol);
    ui_row_.SetVolume(ui_vol);
    res_row_.SetVolume(ui_vol);
    back_button_->SetVolume(ui_vol);
}

void SettingsState::PrevResolution() {
    // Sfoglia alla risoluzione precedente (senza applicare)
    selected_res_index_ = (selected_res_index_ == 0) 
        ? supported_resolutions_.size() - 1 
        : selected_res_index_ - 1;
    UpdateResLabel();
}

void SettingsState::NextResolution() {
    // Sfoglia alla risoluzione successiva (senza applicare)
    selected_res_index_ = (selected_res_index_ + 1) % supported_resolutions_.size();
    UpdateResLabel();
}

void SettingsState::ApplyResolution() {
    // Applica effettivamente la risoluzione selezionata
    data_->resolution_index = selected_res_index_;
    auto new_mode = supported_resolutions_[selected_res_index_];

    // Ricrea la finestra con la nuova risoluzione
    data_->window.create(new_mode, std::string(Config::Game::kWindowName));
    sf::View view(sf::FloatRect(0, 0, Config::Game::kWindowWidth, Config::Game::kWindowHeight));
    data_->window.setView(view);

    Logger::Info("Risoluzione applicata: {}x{}", new_mode.width, new_mode.height);
    UpdateResLabel();
}

void SettingsState::UpdateResLabel() {
    auto mode = supported_resolutions_[selected_res_index_];

    if (selected_res_index_ != data_->resolution_index) {
        // Risoluzione diversa da quella attuale → label verde "Applica XxY"
        res_row_.label->SetText(std::format("Applica {}x{}", mode.width, mode.height));
        res_row_.label->SetColors(
            Config::GUI::kIdleGreenCol, 
            Config::GUI::kHoverGreenCol, 
            Config::GUI::kActiveGreenCol
        );
    } else {
        // Risoluzione uguale a quella attuale → label grigia normale
        res_row_.label->SetText(std::format("{}: {}x{}", Config::Settings::kRisoluzioneName, mode.width, mode.height));
        res_row_.label->SetColors(
            Config::GUI::kLabelCol, 
            Config::GUI::kLabelCol, 
            Config::GUI::kLabelCol
        );
    }
}
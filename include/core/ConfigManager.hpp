#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <SFML/Graphics.hpp>
#include "audio/AudioSettings.hpp"

// Struttura per contenere tutti i settaggi utente
struct UserSettings {
    sf::Vector2u resolution;
    AudioSettings audio;
};

class ConfigManager {
 public:
    static constexpr std::string_view kDefaultConfigPath = "assets/config/default_settings.json";
    static constexpr std::string_view kSavesDir = "saves";
    static constexpr std::string_view kUserConfigPath = "saves/user_settings.json";

    ConfigManager();
    ~ConfigManager() = default;

    // Carica le configurazioni (prima da user_settings, se fallisce da default)
    std::expected<void, std::string> Load();
    
    // Salva le configurazioni correnti in user_settings.json
    std::expected<void, std::string> Save() const;

    [[nodiscard]] const UserSettings& GetSettings() const;

    // Aggiorna le impostazioni solo in RAM
    void SetSettings(const UserSettings& new_settings);

    // Aggiorna in RAM e Salva immediatamente su disco
    std::expected<void, std::string> UpdateSettings(const UserSettings& new_settings);

    // Converte width e height in resolution index per comodità (se necessario)
    // O meglio, lascia gestire questa logica a GameData / SettingsState.

 private:
    UserSettings current_settings_;

    // Metodi helper interni
    void EnsureSavesDirectoryExists_() const;
    void LoadFromFallback_();
    bool ParseJsonFile_(const std::string& path);
    void PopulateFromJson_(const std::string& json_str); // usa nlohmann/json internamente
};

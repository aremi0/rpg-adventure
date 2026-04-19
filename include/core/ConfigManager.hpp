#pragma once

#include <string_view>
#include <expected>
#include <SFML/System/Vector2.hpp>
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

    ConfigManager() = default;
    ~ConfigManager() = default;

    // Carica le configurazioni con catena di fallback:
    // 1. saves/user_settings.json
    // 2. assets/config/default_settings.json
    // 3. Constants.hpp (hardcoded)
    void Load();

    // Salva le configurazioni correnti in saves/user_settings.json
    std::expected<void, std::string> Save() const;

    [[nodiscard]] const UserSettings& GetSettings() const;

    // Aggiorna le impostazioni solo in RAM (senza salvare su disco)
    void SetSettings(const UserSettings& new_settings);

 private:
    UserSettings current_settings_;

    // Tenta il parsing di un file JSON e ritorna UserSettings o un errore
    [[nodiscard]] std::expected<UserSettings, std::string> LoadFromFile(std::string_view path) const;

    // Costruisce le impostazioni hardcoded da Constants.hpp (ultima risorsa)
    [[nodiscard]] UserSettings BuildHardcodedDefaults() const;

    // Valida e corregge le impostazioni (clampa volumi, verifica risoluzione)
    void Validate(UserSettings& settings) const;

    // Crea la directory saves/ se non esiste
    void EnsureSavesDirectoryExists() const;
};

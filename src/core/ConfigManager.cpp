#include "core/ConfigManager.hpp"
#include "core/Constants.hpp"
#include "utils/Logger.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <format>

namespace fs = std::filesystem;

// ==================== Public API ====================

void ConfigManager::Load() {
    // Livello 1: Tenta di caricare le impostazioni utente
    auto user_result = LoadFromFile(kUserConfigPath);
    if (user_result) {
        current_settings_ = *user_result;
        Validate(current_settings_);
        Logger::Info("Impostazioni caricate da: {}", kUserConfigPath);
        return;
    }
    Logger::Warn("Impossibile caricare {}: {}", kUserConfigPath, user_result.error());

    // Livello 2: Tenta il file di default (assets/config/default_settings.json)
    auto default_result = LoadFromFile(kDefaultConfigPath);
    if (default_result) {
        current_settings_ = *default_result;
        Validate(current_settings_);
        Logger::Info("Impostazioni caricate da fallback: {}", kDefaultConfigPath);
        return;
    }
    Logger::Warn("Impossibile caricare {}: {}", kDefaultConfigPath, default_result.error());

    // Livello 3: Usa i valori hardcoded da Constants.hpp
    current_settings_ = BuildHardcodedDefaults();
    Logger::Warn("Uso impostazioni hardcoded da Constants.hpp");
}

std::expected<void, std::string> ConfigManager::Save() const {
    EnsureSavesDirectoryExists();

    nlohmann::json json;
    json["resolution"]["width"] = current_settings_.resolution.x;
    json["resolution"]["height"] = current_settings_.resolution.y;
    json["volume"]["master"] = current_settings_.audio.master;
    json["volume"]["music"] = current_settings_.audio.music;
    json["volume"]["sfx"] = current_settings_.audio.sfx;
    json["volume"]["ui"] = current_settings_.audio.ui;

    std::string file_path{kUserConfigPath};
    std::ofstream file{file_path};
    if (!file.is_open()) {
        return std::unexpected(
            std::format("Impossibile aprire {} per la scrittura", kUserConfigPath));
    }

    file << json.dump(4);
    if (file.fail()) {
        return std::unexpected(
            std::format("Errore di scrittura su {}", kUserConfigPath));
    }

    Logger::Info("Impostazioni salvate in: {}", kUserConfigPath);
    return {};
}

const UserSettings& ConfigManager::GetSettings() const {
    return current_settings_;
}

void ConfigManager::SetSettings(const UserSettings& new_settings) {
    current_settings_ = new_settings;
}

// ==================== Private Helpers ====================

std::expected<UserSettings, std::string> ConfigManager::LoadFromFile(
    std::string_view path) const {

    std::string file_path{path};
    std::ifstream file{file_path};
    if (!file.is_open()) {
        return std::unexpected(std::format("File non trovato: {}", path));
    }

    nlohmann::json json;
    try {
        file >> json;
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(
            std::format("JSON corrotto in {}: {}", path, e.what()));
    }

    UserSettings settings;
    try {
        settings.resolution.x = json.at("resolution").at("width").get<unsigned int>();
        settings.resolution.y = json.at("resolution").at("height").get<unsigned int>();
        settings.audio.master = json.at("volume").at("master").get<int>();
        settings.audio.music  = json.at("volume").at("music").get<int>();
        settings.audio.sfx    = json.at("volume").at("sfx").get<int>();
        settings.audio.ui     = json.at("volume").at("ui").get<int>();
    } catch (const nlohmann::json::exception& e) {
        return std::unexpected(
            std::format("Campo mancante o invalido in {}: {}", path, e.what()));
    }

    return settings;
}

UserSettings ConfigManager::BuildHardcodedDefaults() const {
    UserSettings settings;
    settings.resolution = {
        Config::Settings::kDefaultWindowWidth,
        Config::Settings::kDefaultWindowHeight
    };
    settings.audio.master = Config::Settings::kDefaultMasterVolume;
    settings.audio.music  = Config::Settings::kDefaultMusicVolume;
    settings.audio.sfx    = Config::Settings::kDefaultSfxVolume;
    settings.audio.ui     = Config::Settings::kDefaultUiVolume;
    return settings;
}

void ConfigManager::Validate(UserSettings& settings) const {
    // --- Validazione Volumi: clampa nel range [0, 100] ---
    auto clamp_volume = [](int& vol, std::string_view name) {
        int original = vol;
        vol = std::clamp(vol,
                         Config::Settings::kMinVolume,
                         Config::Settings::kMaxVolume);
        if (vol != original) {
            Logger::Warn("Volume {} corretto: {} → {}", name, original, vol);
        }
    };

    clamp_volume(settings.audio.master, "master");
    clamp_volume(settings.audio.music,  "music");
    clamp_volume(settings.audio.sfx,    "sfx");
    clamp_volume(settings.audio.ui,     "ui");

    // --- Validazione Risoluzione: deve essere nella whitelist ---
    bool valid_resolution = false;
    for (const auto& [w, h] : Config::Settings::kSupportedResolutions) {
        if (settings.resolution.x == w && settings.resolution.y == h) {
            valid_resolution = true;
            break;
        }
    }

    if (!valid_resolution) {
        Logger::Warn("Risoluzione {}x{} non supportata, uso default {}x{}",
            settings.resolution.x, settings.resolution.y,
            Config::Settings::kDefaultWindowWidth,
            Config::Settings::kDefaultWindowHeight);
        settings.resolution = {
            Config::Settings::kDefaultWindowWidth,
            Config::Settings::kDefaultWindowHeight
        };
    }
}

void ConfigManager::EnsureSavesDirectoryExists() const {
    std::string saves_str{kSavesDir};
    fs::path saves_path{saves_str};
    if (!fs::exists(saves_path)) {
        fs::create_directories(saves_path);
        Logger::Debug("Directory '{}' creata", kSavesDir);
    }
}

#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <expected>
#include "utils/Logger.hpp"
#include "core/Constants.hpp"

// Definiamo i possibili errori di caricamento
enum class AssetError {
    FileNotFound,
    CorruptedFile,
    AlreadyLoaded,
};

class AssetManager {
    public:
        AssetManager() {
            // Creazione texture di fallback (scacchiera viola/nera a blocchi)
            sf::Image fallback_img;
            fallback_img.create(Config::Graphics::kTileSize, Config::Graphics::kTileSize, sf::Color::Magenta);
            const unsigned int block_size = Config::Graphics::kTileSize / 8;

            for (unsigned int y = 0; y < Config::Graphics::kTileSize; ++y) {
                for (unsigned int x = 0; x < Config::Graphics::kTileSize; ++x) {
                    bool is_dark = ((x / block_size) + (y / block_size)) % 2 == 0;
                    sf::Uint8 r = is_dark ? 255 : 0;
                    sf::Uint8 g = 0;
                    sf::Uint8 b = is_dark ? 255 : 0;
                    fallback_img.setPixel(x, y, sf::Color(r, g, b));
                }
            }

            if (!fallback_texture_.loadFromImage(fallback_img)) {
                Logger::Error("ERRORE CRITICO: Impossibile creare la texture di fallback");
            }

            fallback_texture_.setRepeated(true);
        }
        
        ~AssetManager() = default;

        // Metodo Template: T può essere sf::Texture, sf::Font, sf::SoundBuffer...
        template<typename T>
        std::expected<void, AssetError> LoadAsset(const std::string& name, const std::string& file_path) {
            // Se l'asset esiste già, non caricarlo
            if (GetStorage<T>().contains(name)) {
                Logger::Warn("Asset '{}' già caricato", name);
                return std::unexpected(AssetError::AlreadyLoaded);
            }

            T asset;
            if (!asset.loadFromFile(file_path)) {
                Logger::Error("Impossibile caricare l'asset '{}' da: {}", name, file_path);
                return std::unexpected(AssetError::FileNotFound);
            }

            GetStorage<T>()[name] = std::move(asset);
            Logger::Trace("Asset '{}' caricato con successo da: {}", name, file_path);
            return {};
        }

        // sf::Music non può essere copiato e usa openFromFile, quindi lo gestiamo a parte con unique_ptr
        std::expected<void, AssetError> LoadMusic(const std::string& name, const std::string& file_path) {
            if (musics_.contains(name)) {
                Logger::Warn("Musica '{}' già caricata", name);
                return std::unexpected(AssetError::AlreadyLoaded);
            }

            auto music = std::make_unique<sf::Music>();
            if (!music->openFromFile(file_path)) { // Nota: openFromFile, non loadFromFile
                Logger::Error("Impossibile aprire la musica '{}' da: {}", name, file_path);
                return std::unexpected(AssetError::FileNotFound);
            }

            musics_[name] = std::move(music);
            Logger::Trace("Musica '{}' caricata con successo da: {}", name, file_path);
            return {};
        }

        sf::Music& GetMusic(const std::string& name) {
            auto it = musics_.find(name);
            if (it == musics_.end()) {
                Logger::Error("Musica '{}' non trovata.", name);
                throw std::runtime_error("Musica mancante: " + name);
            }
            return *(it->second); // Dereferenzia il unique_ptr
        }

        template<typename T>
        const T& GetAsset(const std::string& name) const {
            auto& storage = GetStorage<T>();
            auto it = storage.find(name);
            
            if(it == storage.end()) {
                if constexpr (std::is_same_v<T, sf::Texture>) {
                    // Gestione specifica per Texture: restituisci la scacchiera
                    Logger::Warn("Texture '{}' non trovata nel manager. Uso fallback", name);
                    return fallback_texture_;
                }
                
                // Per Font o altri asset critici, non c'è un fallback facile -> Eccezione
                Logger::Error("Asset critico mancante: {}", name);
                throw std::runtime_error("Asset critico mancante: " + name);
            }
            return it->second;
        }

    private:
        // Helper per ottenere la mappa corretta
        template<typename T>
        auto& GetStorage() {
            if constexpr (std::is_same_v<T, sf::Texture>) return textures_;
            else if constexpr (std::is_same_v<T, sf::Font>) return fonts_;
            else if constexpr (std::is_same_v<T, sf::SoundBuffer>) return sound_buffers_;
            else static_assert(sizeof(T) == 0, "Tipo di asset non supportato in AssetManager");
        }

        template<typename T>
        const auto& GetStorage() const {
            if constexpr (std::is_same_v<T, sf::Texture>) return textures_;
            else if constexpr (std::is_same_v<T, sf::Font>) return fonts_;
            else if constexpr (std::is_same_v<T, sf::SoundBuffer>) return sound_buffers_;
            else static_assert(sizeof(T) == 0, "Tipo di asset non supportato in AssetManager");
        }

        sf::Texture fallback_texture_;
        std::unordered_map<std::string, sf::Texture> textures_;
        std::unordered_map<std::string, sf::Font> fonts_;
        std::unordered_map<std::string, sf::SoundBuffer> sound_buffers_;
        std::unordered_map<std::string, std::unique_ptr<sf::Music>> musics_;
};
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <expected>
#include "utils/Logger.hpp"

// Definiamo i possibili errori di caricamento
enum class AssetError {
    FileNotFound,
    CorruptedFile,
    AlreadyLoaded,
};

class AssetManager {
    public:
        static constexpr unsigned int kTileSize = 64;

        AssetManager() {
            // Creazione texture di fallback (scacchiera viola/nera a blocchi)
            sf::Image fallback_img;
            fallback_img.create(kTileSize, kTileSize, sf::Color::Magenta);
            const unsigned int block_size = kTileSize / 8;

            for (unsigned int y = 0; y < kTileSize; ++y) {
                for (unsigned int x = 0; x < kTileSize; ++x) {
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
            Logger::Info("Asset '{}' caricato con successo da: {}", name, file_path);
            return {};
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
            else static_assert(sizeof(T) == 0, "Tipo di asset non supportato in AssetManager");
        }

        template<typename T>
        const auto& GetStorage() const {
            if constexpr (std::is_same_v<T, sf::Texture>) return textures_;
            else if constexpr (std::is_same_v<T, sf::Font>) return fonts_;
            else static_assert(sizeof(T) == 0, "Tipo di asset non supportato in AssetManager");
        }

        sf::Texture fallback_texture_;
        std::unordered_map<std::string, sf::Texture> textures_;
        std::unordered_map<std::string, sf::Font> fonts_;
        //std::unordered_map<std::string, sf::SoundBuffer> sounds_;
        //std::unordered_map<std::string, sf::Music> musics_;
};
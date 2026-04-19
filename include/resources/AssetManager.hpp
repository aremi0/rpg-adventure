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
#include <future>
#include <vector>

// Definiamo i possibili errori di caricamento
enum class AssetError {
    FileNotFound,
    CorruptedFile,
};

class AssetManager {
    public:
        AssetManager() {
            // Creazione texture di fallback (scacchiera viola/nera a blocchi)
            sf::Image fallback_img;
            fallback_img.create(Config::Game::Textures::kTileSize, Config::Game::Textures::kTileSize, sf::Color::Magenta);
            const unsigned int block_size = Config::Game::Textures::kTileSize / 8;

            for (unsigned int y = 0; y < Config::Game::Textures::kTileSize; ++y) {
                for (unsigned int x = 0; x < Config::Game::Textures::kTileSize; ++x) {
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

        // --- Caricamento Singolo (Sincrono) ---

        /**
         * @brief Carica un singolo asset di tipo T.
         * T può essere: sf::Texture, sf::Font, sf::SoundBuffer.
         */
        template<typename T>
        std::expected<void, AssetError> LoadAsset(const std::string& name, const std::string& file_path) {
            if (GetStorage<T>().contains(name)) {
                Logger::Trace("Asset '{}' gia' in memoria. Ricaricamento saltato.", name);
                return {};
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

        /**
         * @brief Specializzazione per sf::Music (gestita come streaming/unique_ptr).
         */
        template<typename T> requires std::is_same_v<T, sf::Music>
        std::expected<void, AssetError> LoadAsset(const std::string& name, const std::string& file_path) {
            if (musics_.contains(name)) {
                Logger::Trace("Musica '{}' gia' in memoria. Ricaricamento saltato.", name);
                return {};
            }

            auto music = std::make_unique<sf::Music>();
            if (!music->openFromFile(file_path)) {
                Logger::Error("Impossibile aprire la musica '{}' da: {}", name, file_path);
                return std::unexpected(AssetError::FileNotFound);
            }

            musics_[name] = std::move(music);
            Logger::Trace("Musica '{}' caricata con successo da: {}", name, file_path);
            return {};
        }

        // --- Caricamento Batch (Parallelo) ---

        struct AssetTask {
            std::string name;
            std::string path;
        };

        /**
         * @brief Carica un set di asset in batch, usando il parallelismo dove possibile.
         */
        template<typename T>
        void LoadAsset(const std::vector<AssetTask>& tasks) {
            if constexpr (std::is_same_v<T, sf::Texture>) {
                LoadTexturesParallelInternal(tasks);
            } else if constexpr (std::is_same_v<T, sf::SoundBuffer>) {
                LoadSoundsParallelInternal(tasks);
            } else {
                // Per Font o altri tipi, caricamento sequenziale
                for (const auto& task : tasks) {
                    static_cast<void>(LoadAsset<T>(task.name, task.path));
                }
            }
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


        template<typename T>
        bool HasAsset(const std::string& name) const {
            return GetStorage<T>().contains(name);
        }

        bool HasMusic(const std::string& name) const {
            return musics_.contains(name);
        }

    private:
        // --- Metodi Helper Interni ---

        void LoadTexturesParallelInternal(const std::vector<AssetTask>& tasks) {
            std::vector<AssetTask> pending;
            for (const auto& t : tasks) {
                if (!textures_.contains(t.name)) {
                    pending.push_back(t);
                }
            }

            if (pending.empty()) {
                return;
            }

            std::vector<std::future<std::pair<std::string, sf::Image>>> futures;
            for (const auto& t : pending) {
                futures.push_back(std::async(std::launch::async, [t]() {
                    sf::Image img; 
                    img.loadFromFile(t.path);
                    return std::make_pair(t.name, std::move(img));
                }));
            }

            for (auto& f : futures) {
                auto [name, img] = f.get();
                if (img.getSize().x > 0) {
                    static_cast<void>(LoadTextureFromImage(name, img));
                }
            }
        }

        void LoadSoundsParallelInternal(const std::vector<AssetTask>& tasks) {
            std::vector<AssetTask> pending;
            for (const auto& t : tasks) {
                if (!sound_buffers_.contains(t.name)) {
                    pending.push_back(t);
                }
            }

            if (pending.empty()) {
                return;
            }

            std::vector<std::future<std::pair<std::string, sf::SoundBuffer>>> futures;
            for (const auto& t : pending) {
                futures.push_back(std::async(std::launch::async, [t]() {
                    sf::SoundBuffer buf; 
                    buf.loadFromFile(t.path);
                    return std::make_pair(t.name, std::move(buf));
                }));
            }

            for (auto& f : futures) {
                auto [name, buf] = f.get();
                if (buf.getSampleCount() > 0) {
                    sound_buffers_[name] = std::move(buf);
                }
            }
        }

        std::expected<void, AssetError> LoadTextureFromImage(const std::string& name, const sf::Image& image) {
            if (textures_.contains(name)) return {};
            sf::Texture tex;
            if (!tex.loadFromImage(image)) return std::unexpected(AssetError::CorruptedFile);
            textures_[name] = std::move(tex);
            return {};
        }

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
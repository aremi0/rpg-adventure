## Fase 3: Resource Management (Il cuore visivo)

In un RPG caricherai centinaia di texture, font e suoni. Scrivere `texture.loadFromFile("...")` ogni volta è pericoloso (rischi duplicati in memoria) e inefficiente.

Creeremo un **AssetManager** (o `ResourceHolder`) che sfrutta i template per gestire qualsiasi tipo di risorsa SFML.

### Cosa faremo in questo step:

1. Useremo un `std::unordered_map<std::string, T>` per memorizzare gli asset.
2. Implementeremo un metodo `loadAsset(name, path)`.
3. Useremo `std::expected` per gestire i file mancanti senza crashare.

---

### 1. `include/resources/AssetManager.hpp`

Sfruttiamo i template per avere un unico magazzino per tipo di risorsa.

```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <expected>
#include <memory>
#include "utils/Logger.hpp"

// Definiamo i possibili errori di caricamento
enum class AssetError {
    FileNotFound,
    CorruptedFile,
    AlreadyLoaded
};

class AssetManager {
public:
    AssetManager() = default;

    // Metodo Template: T può essere sf::Texture, sf::Font, sf::SoundBuffer...
    template<typename T>
    std::expected<void, AssetError> loadAsset(const std::string& name, const std::string& filePath) {
        // Se l'asset esiste già, non caricarlo di nuovo
        if (getStorage<T>().contains(name)) {
            LOG_WARN("Asset '{}' già caricato.", name);
            return std::unexpected(AssetError::AlreadyLoaded);
        }

        T asset;
        if (!asset.loadFromFile(filePath)) {
            LOG_ERROR("Impossibile caricare l'asset '{}' da: {}", name, filePath);
            return std::unexpected(AssetError::FileNotFound);
        }

        getStorage<T>()[name] = std::move(asset);
        LOG_INFO("Asset '{}' caricato con successo da: {}", name, filePath);
        return {};
    }

    template<typename T>
    const T& getAsset(const std::string& name) const {
        auto& storage = getStorage<T>();
        auto it = storage.find(name);
        if (it == storage.end()) {
            LOG_ERROR("Asset '{}' non trovato nel manager!", name);
            // In un RPG reale qui potresti restituire una texture "fallback" (es. scacchiera viola/nera)
            throw std::runtime_error("Asset not found: " + name);
        }
        return it->second;
    }

private:
    // Helper per ottenere la mappa corretta in base al tipo
    template<typename T>
    auto& getStorage() {
        if constexpr (std::is_same_v<T, sf::Texture>) return _textures;
        else if constexpr (std::is_same_v<T, sf::Font>) return _fonts;
        // Aggiungi altri tipi qui (es. sf::SoundBuffer)
    }

    template<typename T>
    const auto& getStorage() const {
        if constexpr (std::is_same_v<T, sf::Texture>) return _textures;
        else if constexpr (std::is_same_v<T, sf::Font>) return _fonts;
    }

    std::unordered_map<std::string, sf::Texture> _textures;
    std::unordered_map<std::string, sf::Font> _fonts;
};

```

### Perché questa versione è superiore?

1. **`std::unordered_map`**: E' una Hash Table. La ricerca è O(1) in media. Per un gioco che cerca texture continuamente, è molto più veloce.
2. **`std::expected<void, AssetError>`**: Invece di lanciare eccezioni o ritornare un banale `bool`, restituiamo un oggetto che dice "Tutto ok" oppure "Ecco perché è fallito". Chi chiama il metodo può decidere cosa fare in base all'errore specifico.
3. **`if constexpr`**: Questa è magia del C++ moderno. Il compilatore decide quale mappa usare a tempo di compilazione, zero overhead a runtime.
4. **Template unificato**: Non scriverai mai più `loadTexture`, `loadFont`, `loadSound`. Scriverai solo `loadAsset<sf::Texture>(...)`.

---

### Come lo useresti nel tuo `MainMenuState.cpp`?

```cpp
void MainMenuState::init() {
    // Caricamento moderno con gestione dell'errore (C++23)
    auto result = _data->assets.loadAsset<sf::Texture>("background", "assets/textures/main_menu.png");
    
    if (!result) {
        if (result.error() == AssetError::FileNotFound) {
            // Fai qualcosa se il file manca (es. usa asset di default)
        }
    }

    // Per usarlo:
    _backgroundSprite.setTexture(_data->assets.getAsset<sf::Texture>("background"));
}

```

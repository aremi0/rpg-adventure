## Integrazione: Settings Persistence e Caricamento Asincrono

Per rendere il gioco data-driven e garantire la persistenza delle impostazioni utente, abbiamo introdotto un sistema di salvataggio/caricamento basato su file JSON con validazione completa e una catena di fallback a 3 livelli. Come bonus, il caricamento dei frame del background è stato parallelizzato con `std::async`.

### 1. Catena di Fallback a 3 Livelli

Il `ConfigManager` tenta il caricamento delle impostazioni in ordine di priorità. Se un livello fallisce (file inesistente, JSON corrotto, campi mancanti), scende al livello successivo senza mai crashare.

**`src/core/ConfigManager.cpp`** (Flusso di `Load()`)
- **Livello 1**: `saves/user_settings.json` → impostazioni personalizzate dal giocatore
- **Livello 2**: `assets/config/default_settings.json` → fallback di distribuzione
- **Livello 3**: `Constants.hpp` hardcoded → ultima risorsa, compilato nel binario

### 2. Validazione e Sicurezza dei Valori

Ogni impostazione caricata viene controllata prima dell'uso per evitare valori illegali (volumi negativi, risoluzioni inesistenti) che il giocatore potrebbe aver inserito manualmente nel JSON.

**`src/core/ConfigManager.cpp`** (`Validate()`)
- **Volumi**: clampati nel range `[0, 100]` tramite `std::clamp`
- **Risoluzione**: verificata contro una whitelist constexpr definita in `Config::Settings::kSupportedResolutions`
- Ogni correzione viene loggata con `Logger::Warn` per tracciabilità

### 3. Risoluzione Logica vs Finestra

Il gioco mantiene una **risoluzione logica fissa** (1024×768) per tutti i calcoli UI, separata dalla risoluzione fisica della finestra che è l'unica configurabile dall'utente. Questo garantisce che bottoni e layout restino sempre centrati indipendentemente dalla dimensione della finestra.

**`src/core/Game.cpp`**
```cpp
// Finestra con risoluzione da JSON
data_->window.create(sf::VideoMode(settings.resolution.x, settings.resolution.y), ...);

// View logica fissa — la UI "vive" sempre in 1024x768
sf::View logical_view(sf::FloatRect(0, 0, Config::Game::kLogicalWidth, Config::Game::kLogicalHeight));
data_->window.setView(logical_view);
```

### 4. Pattern RAM-Only e Salvataggio su "Indietro"

Le impostazioni nel `SettingsState` vengono modificate esclusivamente su una copia locale `temp_settings_`. Il salvataggio su disco avviene **solo** alla pressione del bottone "Indietro", evitando scritture inutili e permettendo di annullare implicitamente le modifiche chiudendo la finestra.

### 5. Centralizzazione in GameData

Tutti i dati di configurazione passano per un unico punto: `ConfigManager config` dentro `GameData`. I vecchi campi sparsi (`audio`, `resolution_index`) sono stati eliminati. L'accesso avviene tramite:

```cpp
data_->config.GetSettings().audio.GetMusicVolume()
data_->config.GetSettings().resolution
```

### 6. Bonus: Caricamento Asincrono dei Background Frames

Il caricamento sequenziale dei 48 frame PNG del menu è stato parallelizzato sfruttando il fatto che `sf::Image::loadFromFile()` è thread-safe (nessun OpenGL coinvolto), mentre solo l'upload della texture sulla GPU richiede il thread principale.

**`src/states/MainMenuState.cpp`**
```cpp
// Fase 1: Decodifica PNG in parallelo su tutti i core
std::vector<std::future<sf::Image>> image_futures;
for (const auto& task : frame_tasks) {
    image_futures.push_back(
        std::async(std::launch::async, [path = task.path]() {
            sf::Image img;
            img.loadFromFile(path);
            return img;
        })
    );
}

// Fase 2: Upload GPU seriale (veloce, solo trasferimento RAM → VRAM)
for (size_t i = 0; i < frame_tasks.size(); ++i) {
    sf::Image img = image_futures[i].get();
    data_->assets.LoadTextureFromImage(frame_tasks[i].name, img);
}
```

**`include/resources/AssetManager.hpp`** — Aggiunto il metodo `LoadTextureFromImage()` che crea una `sf::Texture` da un'immagine già decodificata in RAM, completando il pattern *decode-parallelizzato + upload-seriale*.

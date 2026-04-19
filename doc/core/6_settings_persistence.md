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

### 6. Bonus: Caricamento Parallelo via AssetManager

Per non bloccare il caricamento iniziale (specialmente per i 48 frame PNG del menu), l' `AssetManager` implementa internamente il parallelismo. La decodifica delle immagini/audio avviene su thread separati, mentre l'upload sulla GPU rimane serializzato sul thread principale per compatibilità OpenGL.

**Uso Unificato nell' `Init()` degli Stati:**
Il design pattern è ora completamente incapsulato. Lo stato non gestisce più i thread, ma chiede semplicemente il caricamento di un "batch" di asset:

```cpp
// Prepara la lista dei task
std::vector<AssetManager::AssetTask> tasks;
// ... riempimento tasks ...

// Caricamento Parallelo (gestito internamente da AssetManager)
data_->assets.LoadAsset<sf::Texture>(tasks);
```

**`include/resources/AssetManager.hpp`**
- **Sincrono vs Batch**: `LoadAsset` è sovraccaricato per supportare sia il caricamento singolo che quello massivo.
- **Supporto Music**: Unificato tramite `LoadAsset<sf::Music>`, che gestisce automaticamente lo streaming.
- **Cache Check**: Ogni caricamento batch controlla preventivamente se l'asset è già in memoria, evitando thread e I/O inutili.

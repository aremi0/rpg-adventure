## 3.6 Audio System & Settings UI

Documentazione di tutte le modifiche applicate in questa sessione.

---

### 3.6.1 Fix: Suoni non funzionanti nel SettingsState

**Problema**: I suoni hover/click non venivano riprodotti nel `SettingsState`, nonostante il codice fosse identico al `MainMenuState`.

**Causa**: In [AssetManager.hpp](), `LoadAsset()` restituiva `std::unexpected(AssetError::AlreadyLoaded)` quando un asset era già in memoria — trattando un caso legittimo come **errore**. Il `SettingsState` interpretava questo errore e impostava i puntatori a `nullptr`.

**Fix**:
```diff
-return std::unexpected(AssetError::AlreadyLoaded);
+return std::expected<void, AssetError>{};
```

Tutti i `return {};` sostituiti con `return std::expected<void, AssetError>{}` per evitare ambiguità del compilatore. Rimosso `AlreadyLoaded` dall'enum `AssetError`. Aggiunto metodo `HasAsset<T>()` e `HasMusic()`.

---

### 3.6.2 OpenAL Warmup (toggleable)

**Problema**: Ritardo di ~100ms alla prima riproduzione audio, dovuto all'inizializzazione lazy di OpenAL.

**Fix**: In [Game.cpp](), il costruttore crea un buffer silenzioso in memoria (1 campione a 0) e lo riproduce a volume 0, forzando l'init di OpenAL prima di qualsiasi stato.

Controllato da [Constants.hpp]():
```cpp
inline constexpr bool kAudioWarmup = true; // false per disattivare
```

Usato `if constexpr` → quando `false`, il blocco è **eliminato dal compilatore** (zero overhead).

---

### 3.6.3 Sistema Volumi Per Canale

**Prima**: Un singolo volume globale tramite `sf::Listener::setGlobalVolume()`.

**Dopo**: 4 canali indipendenti con formula: `volume_effettivo = (master / 100) × (canale / 100) × 100`

#### [NEW] [AudioSettings.hpp]()

```cpp
struct AudioSettings {
    int master = 100;  // Moltiplica tutto
    int music  = 25;   // Musiche di sottofondo
    int sfx    = 100;  // Effetti mondo (passi, colpi...)
    int ui     = 100;  // Suoni interfaccia (hover, click)

    float GetEffectiveVolume(int channel) const;
    float GetMusicVolume() const;  // = GetEffectiveVolume(music)
    float GetSfxVolume() const;
    float GetUiVolume() const;
};
```

Aggiunta come membro `audio` in `GameData` ([Game.hpp]()).

#### Come funziona `GetEffectiveVolume`

```cpp
[[nodiscard]] float GetEffectiveVolume(int channel_volume) const {
    return (static_cast<float>(master) / 100.0f)      // Passo 1
         * (static_cast<float>(channel_volume) / 100.0f) // Passo 2
         * 100.0f;                                      // Passo 3
}
```

| Passo | Operazione | Esempio (master=80, music=50) | Risultato |
|---|---|---|---|
| 1 | `master / 100.0f` → percentuale master come decimale | 80 / 100 | **0.8** |
| 2 | `channel / 100.0f` → percentuale canale come decimale | 50 / 100 | **0.5** |
| 3 | `× 100.0f` → riporta nel range 0–100 per SFML | 0.8 × 0.5 × 100 | **40.0f** |

**Concetti C++ usati:**
- **`static_cast<float>`**: converte `int` → `float`. Senza cast, `80/100 = 0` in C++ (divisione intera tra due int!). Con cast: `80.0f/100.0f = 0.8f`
- **`[[nodiscard]]`**: attributo C++17 — il compilatore avvisa se chiami la funzione senza usare il valore di ritorno (sarebbe un errore logico)
- **`const`**: la funzione non modifica nessun membro della struct

**Altri esempi:**

| master | canale | Calcolo | Volume effettivo |
|---|---|---|---|
| 100 | 100 | 1.0 × 1.0 × 100 | **100.0f** (massimo) |
| 100 | 50 | 1.0 × 0.5 × 100 | **50.0f** |
| 50 | 50 | 0.5 × 0.5 × 100 | **25.0f** |
| 0 | 100 | 0.0 × 1.0 × 100 | **0.0f** (master muto = tutto muto) |

#### Valori default ([Constants.hpp]())

| Costante | Valore |
|---|---|
| `kDefaultMasterVolume` | 100 |
| `kDefaultMusicVolume` | 25 |
| `kDefaultSfxVolume` | 100 |
| `kDefaultUiVolume` | 100 |
| `kVolumeStep` | 5 |

---

### 3.6.4  Button — Supporto Volume

#### [MODIFY] [Button.hpp]() / [Button.cpp]()

- Aggiunto parametro `float volume = 100.f` al costruttore
- Nuovo metodo `SetVolume(float)` che imposta il volume di `hover_sound_` e `click_sound_`
- Usato dagli stati per aggiornare i volumi in tempo reale

---

### 3.6.5 Settings UI — Arrow Buttons

**Prima**: Bottoni cliccabili che ciclavano i valori.

**Dopo**: Righe con frecce ← → e label non interattiva:
```
[<]   Master: 100%   [>]
[<]   Musica: 25%    [>]
[<]   Effetti: 100%  [>]
[<]   UI: 100%       [>]
[<]   Ris: 1024x768  [>]
       [Indietro]
```

#### [NEW struct] `SettingRow` ([SettingsState.hpp]())

```cpp
struct SettingRow {
    std::unique_ptr<Button> left;   // Freccia ◀ (interattiva, con suoni)
    std::unique_ptr<Button> label;  // Label (NON interattiva)
    std::unique_ptr<Button> right;  // Freccia ▶ (interattiva, con suoni)
};
```

- **Label non interattive**: stessi colori per idle/hover/active (`kLabelCol`), nessun suono
- **Volume**: `std::clamp(channel + delta, min, max)` — niente wrap-around
- Helper `MakeSettingRow()` crea le 3 componenti con layout calcolato dinamicamente.

---

### 3.6.6 Risoluzione — Sfoglia e Applica

**Problema**: Cliccare sulle frecce applicava immediatamente la risoluzione, causando sfarfallii e cambi non voluti. Inoltre, il bottone si decentrava a causa delle misurazioni basate sulla finestra fisica, e la risoluzione resettava tornando nel menù.

**Soluzione (Sfoglia e Applica)**:
Le frecce modificano solo la selezione in anteprima. La label funge da pulsante di conferma ("Applica").

- **Persistenza**: L'indice della risoluzione effettiva (`resolution_index`) è salvato e persiste in `GameData`.
- **Anteprima (Sfoglia)**: Le frecce $\leftarrow$ e $\rightarrow$ modificano solo la variabile locale `selected_res_index_`.
- **Pulsante Applica**: Se l'anteprima (`selected_res_index_`) è diversa dalla risoluzione attuale, la label centrale diventa **verde e interattiva** visualizzando ad esempio `Applica 800x600`.
- **Nuovo metodo `Button::SetColors()`**: Permette di applicare dinamicamente a runtime i colori della label (verde-applica o grigio-passivo).
- **Centratura Logica**: Per evitare che i componenti UI si decentrino, il layout dei vari `State` calcola ora il proprio posizionamento basandosi su `Config::Game::kWindowWidth` (larghezza **logica** della View) anziché sulla dimensione fisica del monitor o della finestra corrente.
- **Fix "Click Istantaneo" (Stuck Button)**: Quando la label ritornava allo stato passivo subito dopo aver applicato la risoluzione, il suo metodo `Button::Update()` veniva interrotto istantaneamente. Questo congelava il flag interno `is_pressed_` su `true`. Alla successiva interazione con le frecce, il codice trovava la label "virtualmente" ancora premuta, innescando l'applicazione immediata della risoluzione. Risolto implementando un metodo `Button::Reset()` chiamato forzatamente su componenti inattivi per pulirne lo stato.

---

### 3.6.7 Volume Sync — MainMenuState

**Problema**: Cambiando i volumi nelle Impostazioni e tornando al menu, i bottoni del MainMenu suonavano ancora al vecchio volume.

**Fix**: [MainMenuState::Resume()]() ora ri-applica i volumi:

```cpp
void MainMenuState::Resume() {
    is_paused_ = false;
    float ui_vol = data_->audio.GetUiVolume();
    play_button_->SetVolume(ui_vol);
    settings_button_->SetVolume(ui_vol);
    exit_button_->SetVolume(ui_vol);
    // + aggiorna volume musica
}
```

---



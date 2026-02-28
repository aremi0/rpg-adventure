## 1. Concetti Base e Tipi Moderni

*   [1.1 Alias di Tipo (`using`)](#11-alias-di-tipo-using)
*   [1.2 Deduzione del tipo (`auto`)](#12-deduzione-del-tipo-auto)
*   [1.3 Stringhe Ottimizzate (`std::string_view`)](#13-stringhe-ottimizzate-stdstring_view)
*   [1.4 Attributi Standard (`[[nodiscard]]` e altri)](#14-attributi-standard-nodiscard-e-altri)

### 1.1 Alias di Tipo (`using`)

```cpp
using GameDataRef = std::shared_ptr<GameData>;
```

*   **Cos'è**: Un nome alternativo per un tipo esistente. È il sostituto moderno di `typedef`.
*   **Perché**: Rende il codice leggibile (`GameDataRef` vs `std::shared_ptr<GameData>`) e facile da mantenere (se cambi il tipo sottostante, cambi solo una riga).

### 1.2 Deduzione del Tipo (`auto`)

La keyword `auto` istruisce il compilatore a dedurre il tipo di una variabile in base alla sua espressione di inizializzazione **a tempo di compilazione**.
Non trasforma il C++ in un linguaggio dinamico (come Python o JavaScript); il tipo è statico, fisso e sicuro, ma non è necessario scriverlo esplicitamente.

### Vantaggi Principali

1. **Garanzia di Inizializzazione:** Non è possibile dichiarare una variabile `auto` senza inizializzarla (evita variabili "spazzatura").
2. **Leggibilità:** Semplifica la sintassi quando i tipi sono molto lunghi o complessi (es. iteratori o lambda).
3. **Refactoring Sicuro:** Se cambi il tipo di ritorno di una funzione, non devi riscrivere manualmente tutte le variabili che ne catturano il risultato.

### Regole di Deduzione (Value vs Reference)

È fondamentale capire come `auto` gestisce copie e riferimenti.

**1. `auto` (Copia per valore)**
Di default, `auto` crea una **copia** del valore e ignora i riferimenti (`&`) e i qualificatori `const` di primo livello.

```cpp
const int& numero = 10;
auto x = numero; // 'x' è di tipo 'int' (NON 'const int&'). È una copia modificabile.

```

**2. `const auto&` (Riferimento costante)**
Quando vuoi evitare la copia (per performance) e garantire l'immutabilità (per sicurezza). È la forma più comune per oggetti complessi (es. stringhe, vettori).

```cpp
std::string messaggio = "Ciao";
const auto& ref = messaggio; // 'ref' è 'const std::string&'. Nessuna copia.

```

**3. `auto&` (Riferimento modificabile)**
Permette di modificare l'oggetto originale.

```cpp
auto& modificabile = messaggio; // 'modificabile' è 'std::string&'.

```

### Esempi Pratici

**Semplificazione di Tipi Complessi:**
Senza `auto`, scorrere una mappa o un vettore richiedeva sintassi verbosa.

```cpp
// VECCHIO C++ (Verboso e prono a errori)
std::map<std::string, int>::iterator it = myMap.begin();

// C++ MODERNO (Pulito)
auto it = myMap.begin();

```

**Pattern AAA (Almost Always Auto):**
Molti sviluppatori moderni consigliano di usare `auto` per la maggior parte delle variabili locali (salvo i tipi primitivi semplici come `int` o `bool` dove l'esplicitazione aiuta la lettura immediata), spostando l'attenzione dal *tipo* al *valore*.

```cpp
// Invece di:
GameDataRef data = std::make_shared<GameData>();

// Si preferisce:
auto data = std::make_shared<GameData>();

```

### 1.3 Stringhe Ottimizzate (`std::string_view`)

```cpp
void LoadTexture(std::string_view path);
```

Spesso le funzioni prendono stringhe solo per leggerle.
*   **Il Problema**:
    *   `void Load(std::string path)`: Crea una COPIA (lento).
    *   `void Load(const std::string& path)`: Se passi `"player.png"` (C-string), crea una stringa temporanea.
*   **La Soluzione**: `string_view` (C++17).
    *   È solo una "vista" (puntatore + lunghezza). Non possiede memoria.
    *   Accetta `std::string`, `const char*`, e letterali a costo zero.
    *   **Attenzione**: Non usarlo per memorizzare dati a lungo termine, ma solo come parametro di funzione.

### 1.4 Attributi Standard (`[[nodiscard]]` e altri)

A partire dal C++11 (e ampliati nel C++17/C++20), il linguaggio ha introdotto una sintassi standardizzata per gli **attributi** usando le doppie parentesi quadre `[[ ]]`. Servono a fornire istruzioni opzionali ma preziose al compilatore, permettendo di generare avvisi o ottimizzare il codice, senza alterare il funzionamento logico dell'applicazione.

#### `[[nodiscard]]` (C++17)
Di gran lunga il più importante a livello architetturale. L'attributo chiede al compilatore di generare un *warning* (spesso trattato come errore nei progetti ben configurati) se il valore restituito da una funzione o da una classe viene ignorato dal chiamante.
È essenziale per prevenire **bug logici**, ad esempio su funzioni che non modificano lo stato ma ritornano un calcolo puro, oppure nei meccanismi di gestione errori (vedi [Capitolo 6](6_gestione_degli_errori_e_tipi_monadici.md)).

```cpp
[[nodiscard]] float GetEffectiveVolume() const {
    return (master / 100.0f) * (channel / 100.0f) * 100.0f;
}

void Update() {
    GetEffectiveVolume(); // BUG: Il compilatore lancerà un warning! Il calcolo sta andando perso.
    float v = GetEffectiveVolume(); // OK
}
```

> [!NOTE] 
> Se un intero `struct` o `class` viene marcato come `[[nodiscard]]`, qualsiasi funzione che ritorni un oggetto di quel tipo erediterà implicitamente il comportamento `[[nodiscard]]`. (Ad esempio, il tipo di libreria `std::expected` è intrinsecamente `[[nodiscard]]`).

#### Altri attributi comuni nel C++ Moderno:
*   **`[[maybe_unused]]` (C++17)**: Sopprime i warning del compilatore per "unreachable code" o "variable unused". È utilissimo quando dichiari variabili che usi solo dentro delle macro `assert()` o in specifici branch di pre-compilazione (`#ifdef DEBUG`).
*   **`[[fallthrough]]` (C++17)**: Si inserisce appositamente nei costrutti `switch` dove ometti volontariamente il `break;` per "cadere" nel `case` successivo. Disabilita il fastidioso (ma utile) avviso del compilatore per i break dimenticati.
*   **`[[deprecated("Messaggio")]]` (C++14)**: Segnala ad altri sviluppatori (con tanto di warning esplicito in build) che una vecchia funzione/classe sta per essere rimossa dal progetto e di farne uso di una più moderna.

---
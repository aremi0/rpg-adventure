## 6. Gestione degli Errori e Tipi Monadici

*   [6.1 Il paradigma `std::expected` vs Eccezioni](#61-il-paradigma-stdexpected-vs-eccezioni)
*   [6.2 Utilizzo di `std::unexpected**](#62-utilizzo-di-stdunexpected)

### 6.1 Il tipo `std::expected`

Introdotto nello standard C++23 (header `<expected>`), `std::expected<T, E>` è un tipo "vocabolario" che rappresenta il risultato di un'operazione che può avere successo (restituendo un valore di tipo `T`) oppure fallire (restituendo un errore di tipo `E`).

A differenza delle eccezioni (`try-catch`), che interrompono il flusso di esecuzione e sono costose in termini di performance, e a differenza dei codici di errore (che spesso ignorano il valore di ritorno), `std::expected` obbliga il programmatore a gestire esplicitamente il caso di fallimento, mantenendo il codice lineare e performante.

> [!TIP]
> **Collegamento con `[[nodiscard]]`**: Nello standard C++, il tipo `std::expected` è marcato implicitamente con l'attributo `[[nodiscard]]` (vedi [Capitolo 1.4](1_concetti_base_e_tipi_moderni.md#14-attributi-standard-nodiscard-e-altri)). Questo significa che il compilatore segnalerà obbligatoriamente un warning se chiami una funzione fallibile ma dimentichi di gestirne l'eventuale errore restituito.

**Sintassi:**

```cpp
std::expected<T, E>

```

* **T (Type):** Il tipo del valore restituito in caso di successo (può essere `void`).
* **E (Error):** Il tipo dell'errore restituito in caso di fallimento.

### 6.2 Utilizzo di `std::unexpected`

Quando una funzione deve segnalare un errore, non può restituire direttamente un oggetto di tipo `E` (perché il compilatore non saprebbe distinguerlo da `T` in alcuni casi). Si utilizza quindi il wrapper `std::unexpected` per "impacchettare" l'errore.

**Esempio di Implementazione (Asset Manager):**

```cpp
#include <expected>
#include <string>

enum class AssetError {
    FileNotFound,
    ParseError,
    CorruptedData
};

// Funzione che tenta di caricare una texture
// Ritorna void se successo, AssetError se fallisce
std::expected<void, AssetError> loadTexture(const std::string& path) {
    if (!file_exists(path)) {
        // Restituisce l'errore impacchettato
        return std::unexpected(AssetError::FileNotFound); 
    }
    
    if (!parse_texture_data(path)) {
        return std::unexpected(AssetError::ParseError);
    }

    // Successo: restituisce un oggetto expected "vuoto" (poiché T è void)
    return {}; 
}

```

**Esempio di Utilizzo (Chiamante):**
L'oggetto restituito può essere valutato come un booleano (`true` se contiene il valore, `false` se contiene l'errore).

```cpp
auto result = loadTexture("hero.png");

if (result) {
    // Successo: possiamo procedere
    LOG("Texture caricata!");
} else {
    // Fallimento: accediamo all'errore con .error()
    AssetError err = result.error();
    LOG("Errore critico: {}", static_cast<int>(err));
}

```

**Nota sui metodi monadici (Fluent Interface):**
`std::expected` supporta metodi come `.and_then()` e `.or_else()` che permettono di concatenare operazioni in modo funzionale, evitando una "piramide" di istruzioni `if`.

---
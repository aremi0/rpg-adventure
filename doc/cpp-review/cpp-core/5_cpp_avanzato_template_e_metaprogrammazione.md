## 5. C++ Avanzato: Template e Metaprogrammazione

*   [5.1 L-values vs R-values](#51-l-values-vs-r-values)
*   [5.2 Template Variadici](#52-template-variadici)
*   [5.3 Universal References e Perfect Forwarding](#53-universal-references-e-perfect-forwarding)
*   [5.4 Compile-Time If (`if constexpr`)](#54-compile-time-if-if-constexpr)

Analizziamo questa funzione magica da `Logger.hpp`:

```cpp
template<typename... Args> 
static void info(LogFormat fmt, Args&&... args) 
{ log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
```

### 5.1 L-values vs R-values

Per capire il resto, serve questa distinzione fondamentale:
*   **L-value (Locator Value)**: Ha un nome e un indirizzo in memoria. Persiste oltre la singola espressione.
    *   Es: `int x = 5;` -> `x` è un l-value.
*   **R-value (Right Value)**: È temporaneo, non ha nome, sta per "sparire".
    *   Es: `5`, `x + 2`, `getLocation()`.
*   **Perché importa?**: Gli R-value possono essere **spostati** (`move`) invece che copiati, rubando le loro risorse (molto efficiente).

### 5.2 Template Variadici

```cpp
template<typename... Args>
```
*   **Variadic Templates**: Permettono alla funzione di accettare un numero arbitrario di argomenti, di tipi diversi.
*   **`Args`**: È un *Template Parameter Pack* (un pacchetto di tipi).
*   **`args`** (nella firma della funzione): È un *Function Parameter Pack* (un pacchetto di valori).
*   **`args...`** (nel corpo): È l'operatore di **espansione**. Spacchetta gli argomenti separandoli da virgole.
    Se chiami `info(fmt, a, b, c)`, `log(..., args...)` diventa `log(..., a, b, c)`.

### 5.3 Universal References e Perfect Forwarding

```cpp
Args&&... args
```
Qui `&&` ha un significato speciale. Normalmente `T&&` è una R-value reference. Ma nei template, quando il tipo `T` deve essere dedotto, `T&&` diventa una **Universal Reference** (o Forwarding Reference).
*   Se passi un L-value, `Args` diventa `T&`.
*   Se passi un R-value, `Args` diventa `T&&`.
*   Accetta **tutto**.

```cpp
std::forward<Args>(args)...
```
*   **Il Problema**: Appena dai un nome a un parametro (anche se era un R-value), dentro la funzione quel parametro ha un nome (`args`), quindi diventa un L-value! Se lo passassi direttamente a `log()`, perderesti l'informazione che originariamente era un oggetto temporaneo movibile.
*   **La Soluzione**: `std::forward` è un cast condizionale.
    *   Se `args` era un L-value -> lo passa come L-value.
    *   Se `args` era un R-value -> lo casta di nuovo a R-value (permettendo il move).

Questo meccanismo (**Perfect Forwarding**) permette alla funzione `info` di essere completamente trasparente: passa gli argomenti a `log` *esattamente* come li ha ricevuti, senza copie extra non necessarie.

### 5.4 Compile-Time If (`if constexpr`)

Introdotto in C++17, `if constexpr` è un costrutto condizionale valutato **durante la compilazione**, non mentre il gioco è in esecuzione.

*   **A cosa serve**: Permette di scrivere funzioni generiche che si comportano diversamente in base al tipo `T`, scartando totalmente i rami di codice non validi per quel tipo.
*   **Type Traits**: Spesso usato con tratti di tipo come `std::is_same_v<T, U>`, che restituisce `true` se `T` e `U` sono lo stesso tipo.

**Esempio (AssetManager):**
```cpp
template<typename T>
auto& GetStorage() {
    if constexpr (std::is_same_v<T, sf::Texture>) {
        return textures_; // Compilato SOLO se T è sf::Texture
    } else if constexpr (std::is_same_v<T, sf::Font>) {
        return fonts_;    // Compilato SOLO se T è sf::Font
    }
    // I rami non validi vengono eliminati dal binario finale.
}
```

---
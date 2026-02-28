# C++23 e Architettura di Gioco - Appunti di Viaggio

Un compendio delle scelte architetturali e delle feature C++ utilizzate nel progetto RPG Adventure, partendo dalle basi fino ai concetti avanzati.

## Indice

1.  [Concetti Base e Tipi Moderni](#1-concetti-base-e-tipi-moderni)
    *   [1.1 Alias di Tipo (`using`)](#11-alias-di-tipo-using)
    *   [1.2 Deduzione del tipo (`auto`)](#12-deduzione-del-tipo-auto)
    *   [1.3 Stringhe Ottimizzate (`std::string_view`)](#13-stringhe-ottimizzate-stdstring_view)
2.  [Progettazione delle Classi (Costruzione e Distruzione)](#2-progettazione-delle-classi-costruzione-e-distruzione)
    *   [2.1 Inizializzazione Uniforme e Most Vexing Parse](#21-inizializzazione-uniforme-e-most-vexing-parse)
    *   [2.2 Member Initializer List](#22-member-initializer-list)
    *   [2.3 Metodi Speciali (`default` e `delete`)](#23-metodi-speciali-default-e-delete)
    *   [2.4 Distruttori Virtuali](#24-distruttori-virtuali)
    *   [2.5 Const Overloading (Metodi `const`)](#25-const-overloading-metodi-const)
3.  [Gestione della Memoria (Smart Pointers)](#3-gestione-della-memoria-smart-pointers)
    *   [3.1 Dereferenziazione: Puntatori vs Reference](#31-dereferenziazione-puntatori-vs-reference)
    *   [3.2 unique_ptr vs shared_ptr](#32-unique_ptr-vs-shared_ptr)
    *   [3.3 Creazione (Factory Methods)](#33-creazione-factory-methods)
    *   [3.4 Ownership e Spostamento](#34-ownership-e-spostamento)
4.  [Architettura e Polimorfismo](#4-architettura-e-polimorfismo)
    *   [4.1 Interfacce e Classi Astratte](#41-interfacce-e-classi-astratte)
    *   [4.2 Metodi Opzionali (Hook)](#42-metodi-opzionali-hook)
5.  [C++ Avanzato: Template e Metaprogrammazione](#5-c-avanzato-template-e-metaprogrammazione)
    *   [5.1 L-values vs R-values](#51-l-values-vs-r-values)
    *   [5.2 Template Variadici](#52-template-variadici)
    *   [5.3 Universal References e Perfect Forwarding](#53-universal-references-e-perfect-forwarding)
    *   [5.4 Compile-Time If (`if constexpr`)](#54-compile-time-if-if-constexpr)
6.  [Gestione degli Errori e Tipi Monadici (C++23)](#6-gestione-degli-errori-e-tipi-monadici-c23)
    *   [6.1 Il paradigma `std::expected` vs Eccezioni](#61-il-paradigma-stdexpected-vs-eccezioni)
    *   [6.2 Utilizzo di `std::unexpected**](#62-utilizzo-di-stdunexpected)

1.  [Il Loop di Gioco](#1-il-loop-di-gioco)

### Dove inserirlo nell'Indice

Ti consiglio di inserirlo nel **Capitolo 1**, subito dopo gli Alias. È un concetto base fondamentale prima di passare a cose più complesse.

**1. Concetti Base e Tipi Moderni**

* 1.1 Alias di Tipo (`using`)
* **1.2 Deduzione del Tipo (`auto`)** <-- *Nuova sezione*
* 1.3 Stringhe Ottimizzate (`std::string_view`)

---

### Contenuto per gli Appunti

Ecco la spiegazione formale:

## 1.2 Deduzione del Tipo (`auto`)

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
    *   [1.1 Delta Time (`dt`)](#11-delta-time-dt)

---

## 1. Concetti Base e Tipi Moderni

### 1.1 Alias di Tipo (`using`)

```cpp
using GameDataRef = std::shared_ptr<GameData>;
```

*   **Cos'è**: Un nome alternativo per un tipo esistente. È il sostituto moderno di `typedef`.
*   **Perché**: Rende il codice leggibile (`GameDataRef` vs `std::shared_ptr<GameData>`) e facile da mantenere (se cambi il tipo sottostante, cambi solo una riga).

### 1.2 Deduzione del tipo (`auto`)

```cpp
auto texture = std::make_unique<sf::Texture>();
```

*   **Cos'è**: `auto` permette al compilatore di dedurre il tipo di una variabile dal valore che le viene assegnato.
*   **Perché**: Rende il codice più conciso e leggibile, specialmente quando si lavora con tipi complessi come `std::unique_ptr` o `std::shared_ptr`.

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

---

## 2. Progettazione delle Classi (Costruzione e Distruzione)

### 2.1 Inizializzazione Uniforme e Most Vexing Parse

In C++ moderno (dal C++11 in poi), l'uso delle parentesi graffe `{}` per inizializzare gli oggetti è diventato lo standard raccomandato (pratica nota come *Uniform Initialization* o *Brace Initialization*). Questo approccio risolve un'ambiguità storica del linguaggio e previene conversioni di tipo pericolose.

#### Il "Most Vexing Parse" (L'analisi sintattica più fastidiosa)

Il motivo principale per preferire le `{}` quando si istanzia un oggetto sul momento (es. nello stack) è evitare che il compilatore scambi la creazione di un oggetto per la dichiarazione di una funzione.

**L'Ambiguità (con le tonde):**

```cpp
// ATTENZIONE: Il compilatore legge questa riga come la dichiarazione di una
// funzione chiamata 'game' che non accetta parametri e restituisce un tipo 'Game'.
// L'oggetto NON viene creato in memoria!
Game game(); 

```

**La Soluzione (con le graffe):**

```cpp
// SICURO: Non c'è ambiguità. Il compilatore istanzia immediatamente 
// un oggetto 'Game' chiamando il suo costruttore di default.
Game game{}; 

```

#### I "Superpoteri" delle parentesi graffe

Oltre a risolvere l'ambiguità sintattica, l'inizializzazione uniforme offre due vantaggi fondamentali per la sicurezza del codice:

1. **Prevenzione del "Narrowing" (Restringimento silente):** Le parentesi graffe sono molto severe sui tipi e impediscono la perdita accidentale di dati.
* `int x(4.5);` ➔ Compila senza errori, ma il valore viene troncato a `4`.
* `int x{4.5};` ➔ **Errore di compilazione!** Il C++ blocca l'operazione perché un `float` non entra in un `int` senza perdere decimali.


2. **Inizializzazione sicura a zero:** Scrivere `int punteggio{};` o `bool is_running{};` assicura che le variabili partano rispettivamente da `0` e da `false`. Se non le inizializzassimo, conterrebbero i dati "spazzatura" casuali rimasti in quel blocco di RAM.

### 2.2 Member Initializer List

```cpp
Game::Game(...) : _data(std::make_shared<GameData>()) { ... }
```

I due punti `:` prima di `{}` introducono la lista di inizializzazione.
*   **Regola**: Inizializza i membri qui, non assegnarli nel corpo del costruttore.
*   **Vantaggio**: Evita la "doppia inizializzazione" (costruzione di default + assegnazione) ed è obbligatorio per riferimenti (`&`) e costanti (`const`).

### 2.3 Metodi Speciali (`default` e `delete`)

In C++ moderno possiamo controllare esplicitamente i metodi generati dal compilatore.

#### `= delete` (Vietare)
```cpp
class Logger {
public:
    Logger() = delete; // Non puoi creare istanze di Logger!
    // ... metodi statici ...
};
```
*   **Uso**: Serve per classi puramente statiche (come `Logger`) o per impedire la copia (`UniqueHandle(const UniqueHandle&) = delete;`).
*   In passato si mettevano i costruttori in `private`, ora si usa `= delete` che è più chiaro e genera errori di compilazione migliori.

#### `= default` (Richiedere)
```cpp
State() = default;
```
*   **Uso**: Dice al compilatore "Generami l'implementazione standard di questo metodo". Utile quando hai scritto un altro costruttore (che sopprimerebbe quello di default) ma ti serve anche quello vuoto.

### 2.4 Distruttori Virtuali

```cpp
virtual ~State() = default;
```
*   **Regola D'Oro**: Se una classe ha anche solo un metodo `virtual`, DEVE avere un distruttore virtuale.
*   **Perché**: Permette di distruggere correttamente un oggetto derivato (`MenuState`) tramite un puntatore alla base (`State*`). Senza di esso, verrebbe chiamato solo il distruttore base e i membri di `MenuState` rimarrebbero in memoria (Memory Leak).

### 2.5 Const Overloading (Metodi `const`)

È possibile avere due metodi con lo stesso nome e gli stessi parametri, differenziati solo dalla parola chiave `const` alla fine.
*   **Accesso in Lettura**: `const T& Get() const` viene chiamato su oggetti `const` (o tramite riferimenti `const`). Garantisce che l'oggetto non venga modificato.
*   **Accesso in Scrittura**: `T& Get()` viene chiamato su oggetti modificabili. Restituisce un riferimento modificabile.

**Esempio (AssetManager):**
```cpp
// Versione modificabile (usata per caricare asset)
// auto& GetStorage() { ... }

// Versione sola lettura (usata per disegnare o ottenere asset)
// const auto& GetStorage() const { ... }
```
Il compilatore sceglie automaticamente la versione corretta in base al contesto.

---

## 3. Gestione della Memoria (Smart Pointers)

### 3.1 Dereferenziazione: Puntatori vs Reference (`*` vs `&`)

In C++, c'è una netta distinzione sintattica e concettuale tra l'indirizzo di memoria di un oggetto e l'oggetto stesso. Questa differenza è fondamentale quando si estraggono dati dai contenitori (come le mappe) o dagli Smart Pointers.

* **Puntatore (`T*` o `unique_ptr<T>`)**: È una variabile ("una scatola") che *contiene* l'indirizzo di memoria dell'oggetto. Può essere nullo (`nullptr`). Per accedere ai metodi dell'oggetto puntato si usa la freccia `->`.
* **Reference (`T&`)**: È un *alias* (un soprannome) per un oggetto che già esiste. Viene trattato sintatticamente come l'oggetto nudo e crudo (si usa il punto `.`). Non può mai essere nullo. Passare un oggetto per reference evita di farne una copia pesante.

#### Estrarre valori dagli Smart Pointers (L'operatore `*`)

Quando una funzione promette di restituire una reference (`T&`), non possiamo restituirle direttamente uno Smart Pointer. Dobbiamo "aprire" il puntatore ed estrarre l'oggetto al suo interno usando l'operatore di **dereferenziazione** `*` (asterisco).

**Esempio (AssetManager con `sf::Music`):**
Poiché gli oggetti `sf::Music` sono pesanti e non copiabili, vengono spesso memorizzati tramite `std::unique_ptr` all'interno di una mappa.

```cpp
// Promettiamo di restituire la musica vera e propria per riferimento (&)
sf::Music& GetMusic(const std::string& name) {
    auto it = musics_.find(name);
    
    // ... controlli di errore (it == musics_.end()) ...

    // it->second è di tipo std::unique_ptr<sf::Music>. Agisce come un puntatore.
    
    // SBAGLIATO: return it->second; 
    // (Errore: il compilatore non può trasformare un puntatore in una reference all'oggetto)

    // CORRETTO: 
    return *(it->second); 
    // L'asterisco "apre" lo unique_ptr e restituisce l'oggetto sf::Music vero e proprio,
    // che il C++ lega automaticamente alla reference di ritorno.
}

```

#### Regola Pratica per i Contenitori:

1. **Se la mappa memorizza oggetti diretti** (es. `std::unordered_map<string, sf::Texture>`):
Non serve l'asterisco. Restituisci direttamente il valore:
`return it->second;`
2. **Se la mappa memorizza puntatori** (es. `std::unordered_map<string, std::unique_ptr<sf::Music>>`):
Devi dereferenziare per ottenere l'oggetto:
`return *(it->second);`

### 3.2 `unique_ptr` vs `shared_ptr`

*   **`std::unique_ptr`**: Possesso Esclusivo ("Questo oggetto è MIO"). Non si può copiare, solo spostare. È leggero come un puntatore raw.
*   **`std::shared_ptr`**: Possesso Condiviso ("Siamo tutti proprietari"). Usa un *Control Block* per contare i riferimenti. L'oggetto muore solo quando l'ultimo `shared_ptr` viene distrutto.

### 3.3 Creazione (Factory Methods)

Evitare `new` diretto.
*   **`std::make_unique<T>()`**: Sicuro (exception safe).
*   **`std::make_shared<T>()`**: Efficiente. Alloca oggetto e contatore in un unico blocco di memoria (meno frammentazione, più cache-friendly).

### 3.4 Ownership e Spostamento

Come passare gli smart pointers tra funzioni?
1.  **Spostare ownership** (per `unique_ptr` o per trasferire `shared_ptr`):
    ```cpp
    func(std::move(ptr)); // ptr ora è vuoto
    ```
2.  **Condividere ownership** (solo `shared_ptr`):
    ```cpp
    func(ptr); // copia il ptr, incrementa ref count
    ```
3.  **Osservare** (senza ownership):
    Passa `const std::unique_ptr<T>&` raramante. Meglio passare `T*` o `T&` se la funzione deve solo *usare* l'oggetto senza gestirne la vita.

---

## 4. Architettura e Polimorfismo

### 4.1 Interfacce e Classi Astratte

In C++, un'interfaccia è una classe con metodi **virtuali puri**:
```cpp
virtual void init() = 0;
```
*   `= 0` rende la classe **Astratta**: non può essere istanziata.
*   Costringe le classi figlie a fornire un'implementazione concreta.

### 4.2 Metodi Opzionali (Hook)

```cpp
virtual void pause() {}
```
Un metodo virtuale con corpo vuoto `{}` fornisce un'implementazione di default "che non fa nulla". Le classi figlie possono fare override se serve, ma non sono obbligate.

---

## 5. C++ Avanzato: Template e Metaprogrammazione

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

## 6. Gestione degli Errori e Tipi Monadici (C++23)

### 6.1 Il tipo `std::expected`

Introdotto nello standard C++23 (header `<expected>`), `std::expected<T, E>` è un tipo "vocabolario" che rappresenta il risultato di un'operazione che può avere successo (restituendo un valore di tipo `T`) oppure fallire (restituendo un errore di tipo `E`).

A differenza delle eccezioni (`try-catch`), che interrompono il flusso di esecuzione e sono costose in termini di performance, e a differenza dei codici di errore (che spesso ignorano il valore di ritorno), `std::expected` obbliga il programmatore a gestire esplicitamente il caso di fallimento, mantenendo il codice lineare e performante.

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



---

## 1. Il Loop di Gioco

### 1.1 Delta Time (`dt`)

```cpp
const float dt = 1.0f / 60.0f;
```
Il tempo trascorso tra un frame e l'altro. Usare `dt` nei calcoli di movimento (`pos += vel * dt`) rende la velocità del gioco indipendente dal frame rate (o frame-rate independent).
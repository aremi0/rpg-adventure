# C++23 e Architettura di Gioco - Appunti di Viaggio

Un compendio delle scelte architetturali e delle feature C++ utilizzate nel progetto RPG Adventure, partendo dalle basi fino ai concetti avanzati.

## Indice

1.  [Concetti Base e Tipi Moderni](#1-concetti-base-e-tipi-moderni)
    *   [1.1 Alias di Tipo (`using`)](#11-alias-di-tipo-using)
    *   [1.2 Stringhe Ottimizzate (`std::string_view`)](#12-stringhe-ottimizzate-stdstring_view)
2.  [Progettazione delle Classi (Costruzione e Distruzione)](#2-progettazione-delle-classi-costruzione-e-distruzione)
    *   [2.1 Member Initializer List](#21-member-initializer-list)
    *   [2.2 Metodi Speciali (`default` e `delete`)](#22-metodi-speciali-default-e-delete)
    *   [2.3 Distruttori Virtuali](#23-distruttori-virtuali)
3.  [Gestione della Memoria (Smart Pointers)](#3-gestione-della-memoria-smart-pointers)
    *   [3.1 unique_ptr vs shared_ptr](#31-unique_ptr-vs-shared_ptr)
    *   [3.2 Creazione (Factory Methods)](#32-creazione-factory-methods)
    *   [3.3 Ownership e Spostamento](#33-ownership-e-spostamento)
4.  [Architettura e Polimorfismo](#4-architettura-e-polimorfismo)
    *   [4.1 Interfacce e Classi Astratte](#41-interfacce-e-classi-astratte)
    *   [4.2 Metodi Opzionali (Hook)](#42-metodi-opzionali-hook)
5.  [Il Loop di Gioco](#5-il-loop-di-gioco)
    *   [5.1 Delta Time (`dt`)](#51-delta-time-dt)
6.  [C++ Avanzato: Template e Metaprogrammazione](#6-c-avanzato-template-e-metaprogrammazione)
    *   [6.1 L-values vs R-values](#61-l-values-vs-r-values)
    *   [6.2 Template Variadici](#62-template-variadici)
    *   [6.3 Universal References e Perfect Forwarding](#63-universal-references-e-perfect-forwarding)

---

## 1. Concetti Base e Tipi Moderni

### 1.1 Alias di Tipo (`using`)

```cpp
using GameDataRef = std::shared_ptr<GameData>;
```

*   **Cos'è**: Un nome alternativo per un tipo esistente. È il sostituto moderno di `typedef`.
*   **Perché**: Rende il codice leggibile (`GameDataRef` vs `std::shared_ptr<GameData>`) e facile da mantenere (se cambi il tipo sottostante, cambi solo una riga).

### 1.2 Stringhe Ottimizzate (`std::string_view`)

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

### 2.1 Member Initializer List

```cpp
Game::Game(...) : _data(std::make_shared<GameData>()) { ... }
```

I due punti `:` prima di `{}` introducono la lista di inizializzazione.
*   **Regola**: Inizializza i membri qui, non assegnarli nel corpo del costruttore.
*   **Vantaggio**: Evita la "doppia inizializzazione" (costruzione di default + assegnazione) ed è obbligatorio per riferimenti (`&`) e costanti (`const`).

### 2.2 Metodi Speciali (`default` e `delete`)

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

### 2.3 Distruttori Virtuali

```cpp
virtual ~State() = default;
```
*   **Regola D'Oro**: Se una classe ha anche solo un metodo `virtual`, DEVE avere un distruttore virtuale.
*   **Perché**: Permette di distruggere correttamente un oggetto derivato (`MenuState`) tramite un puntatore alla base (`State*`). Senza di esso, verrebbe chiamato solo il distruttore base e i membri di `MenuState` rimarrebbero in memoria (Memory Leak).

---

## 3. Gestione della Memoria (Smart Pointers)

### 3.1 `unique_ptr` vs `shared_ptr`

*   **`std::unique_ptr`**: Possesso Esclusivo ("Questo oggetto è MIO"). Non si può copiare, solo spostare. È leggero come un puntatore raw.
*   **`std::shared_ptr`**: Possesso Condiviso ("Siamo tutti proprietari"). Usa un *Control Block* per contare i riferimenti. L'oggetto muore solo quando l'ultimo `shared_ptr` viene distrutto.

### 3.2 Creazione (Factory Methods)

Evitare `new` diretto.
*   **`std::make_unique<T>()`**: Sicuro (exception safe).
*   **`std::make_shared<T>()`**: Efficiente. Alloca oggetto e contatore in un unico blocco di memoria (meno frammentazione, più cache-friendly).

### 3.3 Ownership e Spostamento

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

## 5. Il Loop di Gioco

### 5.1 Delta Time (`dt`)

```cpp
const float dt = 1.0f / 60.0f;
```
Il tempo trascorso tra un frame e l'altro. Usare `dt` nei calcoli di movimento (`pos += vel * dt`) rende la velocità del gioco indipendente dal frame rate (o frame-rate independent).

---

## 6. C++ Avanzato: Template e Metaprogrammazione

Analizziamo questa funzione magica da `Logger.hpp`:

```cpp
template<typename... Args> 
static void info(LogFormat fmt, Args&&... args) 
{ log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
```

### 6.1 L-values vs R-values

Per capire il resto, serve questa distinzione fondamentale:
*   **L-value (Locator Value)**: Ha un nome e un indirizzo in memoria. Persiste oltre la singola espressione.
    *   Es: `int x = 5;` -> `x` è un l-value.
*   **R-value (Right Value)**: È temporaneo, non ha nome, sta per "sparire".
    *   Es: `5`, `x + 2`, `getLocation()`.
*   **Perché importa?**: Gli R-value possono essere **spostati** (`move`) invece che copiati, rubando le loro risorse (molto efficiente).

### 6.2 Template Variadici

```cpp
template<typename... Args>
```
*   **Variadic Templates**: Permettono alla funzione di accettare un numero arbitrario di argomenti, di tipi diversi.
*   **`Args`**: È un *Template Parameter Pack* (un pacchetto di tipi).
*   **`args`** (nella firma della funzione): È un *Function Parameter Pack* (un pacchetto di valori).
*   **`args...`** (nel corpo): È l'operatore di **espansione**. Spacchetta gli argomenti separandoli da virgole.
    Se chiami `info(fmt, a, b, c)`, `log(..., args...)` diventa `log(..., a, b, c)`.

### 6.3 Universal References e Perfect Forwarding

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
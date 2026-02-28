## 2. Progettazione delle Classi (Costruzione e Distruzione)

*   [2.1 Inizializzazione Uniforme e Most Vexing Parse](#21-inizializzazione-uniforme-e-most-vexing-parse)
*   [2.2 Member Initializer List](#22-member-initializer-list)
*   [2.3 Metodi Speciali (`default` e `delete`)](#23-metodi-speciali-default-e-delete)
*   [2.4 Distruttori Virtuali](#24-distruttori-virtuali)
*   [2.5 Const Overloading (Metodi `const`)](#25-const-overloading-metodi-const)

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
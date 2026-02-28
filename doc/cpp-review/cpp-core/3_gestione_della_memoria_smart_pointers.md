## 3. Gestione della Memoria (Smart Pointers)

*   [3.1 Dereferenziazione: Puntatori vs Reference](#31-dereferenziazione-puntatori-vs-reference)
*   [3.2 unique_ptr vs shared_ptr](#32-unique_ptr-vs-shared_ptr)
*   [3.3 Creazione (Factory Methods)](#33-creazione-factory-methods)
*   [3.4 Ownership e Spostamento](#34-ownership-e-spostamento)

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
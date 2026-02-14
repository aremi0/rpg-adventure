# Naming Conventions usata - **Google C++ Style Guide**.

### 1. Tabella Riassuntiva Rapida

| Entità | Stile | Esempio | Note |
| --- | --- | --- | --- |
| **File** | `snake_case` | `my_useful_class.cc` | Tutto minuscolo, underscore separatori. |
| **Tipi** (Class, Struct, Enum, Alias, Template, Concept) | **PascalCase** | `UrlTable`, `UserId` | Maiuscola iniziale, nessuna underscore. |
| **Funzioni / Metodi** | **PascalCase** | `AddEntry()`, `DeleteUrl()` | **Differenza chiave con la STL**: Google usa la maiuscola! |
| **Variabili Locali / Parametri** | `snake_case` | `table_name`, `num_dns_entries` | Tutto minuscolo. |
| **Data Members (Private/Protected)** | **snake_case_** | `table_name_`, `entries_` | **Trailing underscore obbligatorio**. |
| **Struct Data Members (Public)** | `snake_case` | `packet_id`, `header_size` | Nessun underscore finale (come variabili locali). |
| **Costanti (const / constexpr)** | **kPascalCase** | `kMaxConnections`, `kDefaultPort` | **Prefisso 'k' obbligatorio**. |
| **Namespace** | `snake_case` | `web_search`, `file_system` | Nomi brevi, tutto minuscolo. |
| **Macro** | `SCREAMING_SNAKE` | `MY_MACRO_THAT_SCARE_ME` | Da evitare in C++23 se possibile. |
| **Enumerator (Valori Enum)** | **kPascalCase** | `Color::kRed`, `Color::kBlue` | Come le costanti (precedentemente era `SCREAMING`, ora deprecato). |

---

### 2. Le 3 Regole "Firma" di Google

Queste sono le caratteristiche che rendono il codice "Google Style" immediatamente riconoscibile:

#### A. Il Prefisso `k` per le Costanti

Qualsiasi variabile che è costante per tutta la sua vita (sia `const` statico, sia `constexpr`) deve iniziare con `k` seguito da PascalCase.

```cpp
// Google Style
constexpr int kDaysInWeek = 7;
const std::string kConfigFile = "config.txt";

```

#### B. Funzioni in PascalCase

Al contrario della libreria standard (`std::vector::push_back`), Google usa le maiuscole per i metodi. Questo aiuta a distinguere le funzioni (azioni) dalle variabili (oggetti).

```cpp
// Google Style
my_object.DoSomethingImportant();

```

#### C. Trailing Underscore per i Membri

Serve a distinguere immediatamente se stai toccando una variabile locale o lo stato della classe senza usare `this->`.

```cpp
void SetName(std::string name) {
    name_ = name; // Chiaro: name_ è membro, name è parametro
}

```

---

### 3. Esempio Completo in C++23 (Google Style)

Ecco come appare una classe moderna scritta seguendo rigorosamente queste regole:

```cpp
// file: server_config.h

namespace my_project::network { // Namespace snake_case

// Concept in PascalCase (come i Tipi)
template <typename T>
concept Configurable = requires(T t) {
    t.Load();
};

// Struct pubblica: membri snake_case (sembrano variabili)
struct ConnectionInfo {
    std::string host_name; 
    int port;
};

// Classe principale: PascalCase
class ServerConfig {
 public:
    // Costanti: kPascalCase
    static constexpr int kMaxRetries = 5;

    // Enum: kPascalCase per i valori
    enum class State {
        kIdle,
        kRunning,
        kError
    };

    // Costruttore
    explicit ServerConfig(ConnectionInfo info);

    // Funzioni: PascalCase
    void UpdateConfig(std::string_view new_host);
    
    [[nodiscard]] bool IsValid() const;

 private:
    // Membri privati: snake_case_ (underscore finale)
    ConnectionInfo connection_info_;
    State current_state_ = State::kIdle;
    int retry_count_ = 0;
    
    // Metodo privato helper: PascalCase
    void LogError_(std::string_view message) const; 
    // Nota: Google a volte usa PascalCase anche per metodi privati, 
    // ma alcuni team usano snake_case_ per i metodi privati interni. 
    // La guida ufficiale dice: PascalCase per TUTTE le funzioni.
};

} // namespace my_project::network

```

### 4. Strumenti

Se vuoi applicare questo stile senza impazzire, crea un file `.clang-format` nella root del tuo progetto con questo contenuto:

```yaml
BasedOnStyle: Google
IndentWidth: 2
ColumnLimit: 80

```

*(Nota: Google usa tradizionalmente 2 spazi di indentazione, non 4).*
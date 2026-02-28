# C++23 e Architettura di Gioco - Appunti di Viaggio

Un compendio delle scelte architetturali e delle feature C++ utilizzate nel progetto RPG Adventure, partendo dalle basi fino ai concetti avanzati.

## Indice

1.  [Concetti Base e Tipi Moderni](cpp-review/cpp-core/1_concetti_base_e_tipi_moderni.md)
    *   [1.1 Alias di Tipo (`using`)](cpp-review/cpp-core/1_concetti_base_e_tipi_moderni.md#11-alias-di-tipo-using)
    *   [1.2 Deduzione del tipo (`auto`)](cpp-review/cpp-core/1_concetti_base_e_tipi_moderni.md#12-deduzione-del-tipo-auto)
    *   [1.3 Stringhe Ottimizzate (`std::string_view`)](cpp-review/cpp-core/1_concetti_base_e_tipi_moderni.md#13-stringhe-ottimizzate-stdstring_view)
2.  [Progettazione delle Classi (Costruzione e Distruzione)](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md)
    *   [2.1 Inizializzazione Uniforme e Most Vexing Parse](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md#21-inizializzazione-uniforme-e-most-vexing-parse)
    *   [2.2 Member Initializer List](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md#22-member-initializer-list)
    *   [2.3 Metodi Speciali (`default` e `delete`)](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md#23-metodi-speciali-default-e-delete)
    *   [2.4 Distruttori Virtuali](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md#24-distruttori-virtuali)
    *   [2.5 Const Overloading (Metodi `const`)](cpp-review/cpp-core/2_progettazione_delle_classi_costruzione_e_distruzione.md#25-const-overloading-metodi-const)
3.  [Gestione della Memoria (Smart Pointers)](cpp-review/cpp-core/3_gestione_della_memoria_smart_pointers.md)
    *   [3.1 Dereferenziazione: Puntatori vs Reference](cpp-review/cpp-core/3_gestione_della_memoria_smart_pointers.md#31-dereferenziazione-puntatori-vs-reference)
    *   [3.2 unique_ptr vs shared_ptr](cpp-review/cpp-core/3_gestione_della_memoria_smart_pointers.md#32-unique_ptr-vs-shared_ptr)
    *   [3.3 Creazione (Factory Methods)](cpp-review/cpp-core/3_gestione_della_memoria_smart_pointers.md#33-creazione-factory-methods)
    *   [3.4 Ownership e Spostamento](cpp-review/cpp-core/3_gestione_della_memoria_smart_pointers.md#34-ownership-e-spostamento)
4.  [Architettura e Polimorfismo](cpp-review/cpp-core/4_architettura_e_polimorfismo.md)
    *   [4.1 Interfacce e Classi Astratte](cpp-review/cpp-core/4_architettura_e_polimorfismo.md#41-interfacce-e-classi-astratte)
    *   [4.2 Metodi Opzionali (Hook)](cpp-review/cpp-core/4_architettura_e_polimorfismo.md#42-metodi-opzionali-hook)
5.  [C++ Avanzato: Template e Metaprogrammazione](cpp-review/cpp-core/5_cpp_avanzato_template_e_metaprogrammazione.md)
    *   [5.1 L-values vs R-values](cpp-review/cpp-core/5_cpp_avanzato_template_e_metaprogrammazione.md#51-l-values-vs-r-values)
    *   [5.2 Template Variadici](cpp-review/cpp-core/5_cpp_avanzato_template_e_metaprogrammazione.md#52-template-variadici)
    *   [5.3 Universal References e Perfect Forwarding](cpp-review/cpp-core/5_cpp_avanzato_template_e_metaprogrammazione.md#53-universal-references-e-perfect-forwarding)
    *   [5.4 Compile-Time If (`if constexpr`)](cpp-review/cpp-core/5_cpp_avanzato_template_e_metaprogrammazione.md#54-compile-time-if-if-constexpr)
6.  [Gestione degli Errori e Tipi Monadici](cpp-review/cpp-core/6_gestione_degli_errori_e_tipi_monadici.md)
    *   [6.1 Il paradigma `std::expected` vs Eccezioni](cpp-review/cpp-core/6_gestione_degli_errori_e_tipi_monadici.md#61-il-paradigma-stdexpected-vs-eccezioni)
    *   [6.2 Utilizzo di `std::unexpected`](cpp-review/cpp-core/6_gestione_degli_errori_e_tipi_monadici.md#62-utilizzo-di-stdunexpected)

---

1.  [Il Loop di Gioco](cpp-review/game-design/1_loop_di_gioco.md#1-il-loop-di-gioco)
2.  [Paradigma ECS (Entity Component System)](cpp-review/game-design/2_paradigma_ecs_entity_component_system.md#2-paradigma-ecs-entity-component-system)

---
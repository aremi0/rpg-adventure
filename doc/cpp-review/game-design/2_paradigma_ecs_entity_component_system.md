## 2. Paradigma ECS (Entity Component System)

L'obiettivo del Paradigma ECS è abbandonare l'ereditarietà classica per abbracciare la **Data-Oriented Design** (Programmazione Orientata ai Dati), che è lo standard dell'industria moderna (usato da Unity, Unreal, e persino da Minecraft tramite la libreria EnTT che stiamo per usare).

### Il problema dell'OOP Classico nei giochi

Nella programmazione classica (Object-Oriented), di solito si crea una gerarchia di classi:
Hai una classe base `Entity`. Poi crei `MovingEntity` che eredita da `Entity`. Poi crei `Player` e `Enemy` che ereditano da `MovingEntity`.
*Cosa succede se vuoi un forziere che non si muove, ma ha un inventario (come il Player)?* O duplichi il codice, o crei un groviglio di ereditarietà multipla (il temuto "Diamond Problem"). Inoltre, dal punto di vista della memoria (CPU Cache), gli oggetti sparsi per la RAM rallentano enormemente il gioco.

### La Soluzione: Il pattern ECS

L'ECS divide tutto in tre concetti separati e rigorosi:

1. **Entity (L'ID):** Un'Entità non è una classe, non ha metodi e non ha variabili. È letteralmente solo un numero intero (un ID, es. `Entity 42`). Immaginala come una scatola di cartone vuota con un'etichetta.
2. **Component (I Dati):** Sono struct C++ "pure" (Plain Old Data). Non hanno logica, solo variabili.
* *Esempio:* `TransformComponent { float x, y; }`
* *Esempio:* `HealthComponent { int hp; }`
Se metti il componente `Transform` e il componente `Health` nella scatola `Entity 42`, hai appena creato un oggetto posizionabile e distruttibile! Vuoi creare una roccia? Le dai un `Transform` ma niente `Health`. Vuoi creare un fantasma invisibile? Gli dai `Transform` e `Health` ma niente `Sprite`. **Composizione anziché ereditarietà.**


3. **System (La Logica):** Sono le funzioni che fanno il vero lavoro. Un sistema non ha dati propri, ma "interroga" il mondo.
* *Esempio:* Il `MovementSystem` dice: *"Dammi tutte le Entità che hanno un `Transform` e una `Velocity`. Prendo la loro velocità e la sommo alla loro posizione"*. Se un forziere non ha la `Velocity`, il sistema lo ignora in automatico. Velocissimo ed elegante.


### Cosa faremo materialmente nella Fase 4:

1. **Integrazione di EnTT:** Aggiungeremo il `registry` di EnTT (il database che contiene tutte le scatole e i componenti) al nostro `GameData`.
2. **Creazione dei Componenti Base:** Scriveremo le nostre prime struct (es. `TransformComponent`, `SpriteComponent`).
3. **Il Primo Sistema:** Scriveremo un `RenderSystem` che cercherà in automatico tutto ciò che ha uno Sprite e una Posizione, e lo disegnerà a schermo (senza che l'entità debba avere un metodo `draw()`).
4. **Il GameState:** Creeremo finalmente il nuovo stato del gioco (quello in cui entri cliccando "Nuova Partita"), spawneremo la nostra primissima "Entity 0", le attaccheremo uno sprite e la vedremo comparire a schermo gestita interamente dall'ECS.

**In sintesi, l'obiettivo della Fase 4 è costruire il "Motore Logico".**

---
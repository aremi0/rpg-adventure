## 4. Architettura e Polimorfismo

*   [4.1 Interfacce e Classi Astratte](#41-interfacce-e-classi-astratte)
*   [4.2 Metodi Opzionali (Hook)](#42-metodi-opzionali-hook)

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
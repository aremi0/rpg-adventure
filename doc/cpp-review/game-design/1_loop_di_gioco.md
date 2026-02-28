## 1. Il Loop di Gioco

*   [1.1 Delta Time (`dt`)](#11-delta-time-dt)

### 1.1 Delta Time (`dt`)

```cpp
const float dt = 1.0f / 60.0f;
```
Il tempo trascorso tra un frame e l'altro. Usare `dt` nei calcoli di movimento (`pos += vel * dt`) rende la velocità del gioco indipendente dal frame rate (o frame-rate independent).

---
## 3.7 Asset Pipeline e Audio (SFML)

La gestione degli asset (specialmente l'audio) richiede una separazione netta tra i file sorgenti originali e i file compilati letti dal motore di gioco. Questo previene la perdita di qualità (*Generation Loss*) e ottimizza l'uso della memoria.

### 3.7.1 Gestione Audio: RAM vs Streaming

In SFML, l'audio viene gestito tramite due classi distinte, ognuna ottimizzata per uno scopo specifico:

* **`sf::SoundBuffer` + `sf::Sound` (Per gli SFX Brevi)**
* **Come funziona:** Carica l'intero file audio non compresso direttamente nella memoria RAM.
* **Formato ideale:** `.wav` (Lossless, nessuna decodifica richiesta a runtime).
* **Utilizzo:** Effetti sonori frequenti e sovrapponibili (es. passi, colpi di spada, spari). Garantisce latenza zero, essenziale per il feedback del giocatore.


* **`sf::Music` (Per Temi Musicali e Ambience)**
* **Come funziona:** Non carica tutto il file in RAM. Apre un canale (Stream) e legge il file a piccoli blocchi direttamente dal disco fisso mentre suona.
* **Formato ideale:** `.ogg` (Compresso). Un file `.wav` in streaming occuperebbe troppa banda del disco, mentre l'`.ogg` è leggero e ottimizzato.
* **Utilizzo:** Musiche di sottofondo (BGM) o lunghi suoni ambientali continui (es. pioggia).

### 3.7.2 La Filosofia dell'Asset Pipeline

Non modifichiamo **mai** in modo distruttivo i file direttamente nella cartella del gioco. Utilizziamo invece una "Pipeline" (un tubo di conversione):

1. **Source of Truth (Sorgenti Raw):** Una cartella esterna al gioco (es. `Raw_Audio/`) che contiene solo file originali in altissima qualità (es. `.wav`). Questi file sono intoccabili.
2. **Processamento (Build):** Uno script legge i file Raw, applica filtri (es. normalizzazione del volume) e conversioni di formato.
3. **Compiled Assets (Cartella del Gioco):** I file generati dallo script finiscono in `assets/audio/` dentro il progetto. Questa cartella è "sacrificabile": può essere cancellata e rigenerata in qualsiasi momento dallo script.

### 3.7.3 Automazione: Lo script `normalize_audio.sh`

Per evitare che le musiche o gli effetti abbiano volumi sbilanciati (es. un passo inudibile o un'esplosione assordante), utilizziamo uno script `zsh` con `ffmpeg`.
Lo script applica il filtro `loudnorm` (standard televisivo per la percezione del volume) impostando un tetto massimo di picco (True Peak) a **-6dB** e un volume integrato di **-16 LUFS**.

Inoltre, lo script forza un ricampionamento (Resampling) a 48.000 Hz tramite il parametro `-ar 48000` per fornire al motore audio sottostante SFML dei file già standardizzati a 48kHz, sollevando la CPU e il sistema operativo dal compito di dover
eseguire conversioni matematiche in tempo reale.

Lo script è **idempotente**: grazie al controllo sulle date di modifica (`-nt`, newer than), elabora solo i file nuovi o modificati, ignorando quelli già processati. Questo permette di lanciarlo infinite volte in una frazione di secondo.

**Il Codice dello Script:**
Salvare come `normalize_audio.sh` e renderlo eseguibile con `chmod +x normalize_audio.sh`.

**Utilizzo:**
Lanciare con `-h` o `--help` per una guida sull'utilizzo:

```bash
./build_audio.sh --help

```
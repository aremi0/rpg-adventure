#!/usr/bin/env zsh

# --- DEFINIZIONE NOMI CARTELLE ---
INPUT_DIR_SFX="sfx"
INPUT_DIR_MUSIC="music"

OUTPUT_DIR_BASE="normalized"
OUTPUT_DIR_SFX="$OUTPUT_DIR_BASE/sfx"
OUTPUT_DIR_MUSIC="$OUTPUT_DIR_BASE/music"
# ---------------------------------

# 0. Gestione dell'Help
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    echo "🎵 AUDIO ASSET PIPELINE SCRIPT 🎵"
    echo "==================================="
    echo "💡 Uso: ./normalize_audio.sh <percorso/raw_audio> <percorso/assets/audio>"
    echo ""
    echo "STRUTTURA CARTELLE RICHIESTA:"
    echo "La cartella di input (raw_audio) DEVE contenere le seguenti sottocartelle:"
    echo "  📁 \$1"
    echo "  ├── 📁 $INPUT_DIR_SFX/    (inserisci qui i file .wav per gli effetti sonori)"
    echo "  └── 📁 $INPUT_DIR_MUSIC/  (inserisci qui i file .wav per le musiche)"
    echo ""
    echo "ESEMPIO DI UTILIZZO:"
    echo "  ./normalize_audio.sh ./raw_audio ./assets/audio"
    exit 0
fi

# 1. Controllo Argomenti
if [[ -z "$1" || -z "$2" ]]; then
    echo "❌ Errore: Servono due cartelle."
    echo "💡 Scrivi './normalize_audio.sh --help' per vedere come strutturare le cartelle."
    exit 1
fi

RAW_DIR="$1"
OUT_DIR="$2"

# Controllo se la cartella base di input esiste
if [[ ! -d "$RAW_DIR" ]]; then
    echo "❌ Errore: La cartella di origine '$RAW_DIR' non esiste!"
    exit 1
fi

mkdir -p "$OUT_DIR/$OUTPUT_DIR_SFX"
mkdir -p "$OUT_DIR/$OUTPUT_DIR_MUSIC"

echo "🚀 Inizio pipeline audio..."

# 2. SFX (Da .wav a .wav normalizzato)
if [[ -d "$RAW_DIR/$INPUT_DIR_SFX" ]]; then
    echo "⚔️  Elaborazione SFX..."
    find "$RAW_DIR/$INPUT_DIR_SFX" -type f -iname "*.wav" | while read -r file; do
        filename=$(basename "$file")
        out_file="$OUT_DIR/$OUTPUT_DIR_SFX/$filename"
        
        if [[ ! -f "$out_file" || "$file" -nt "$out_file" ]]; then
            echo "   -> Normalizzo: $filename"
            ffmpeg -y -hide_banner -loglevel error -i "$file" \
                   -af "loudnorm=I=-16:TP=-6:LRA=11" \
                   -ar 48000 \
                   "$out_file" </dev/null
        fi
    done
else
    # IL NUOVO AVVISO SE MANCA LA CARTELLA
    echo "⚠️  Attenzione: Cartella SFX ignorata."
    echo "   Non ho trovato la cartella '$RAW_DIR/$INPUT_DIR_SFX'."
    echo "   Crea questa cartella e mettici i file .wav per normalizzarli."
fi

# 3. MUSICA (Da .wav a .ogg normalizzato e compresso)
if [[ -d "$RAW_DIR/$INPUT_DIR_MUSIC" ]]; then
    echo "🎵 Elaborazione Musica..."
    find "$RAW_DIR/$INPUT_DIR_MUSIC" -type f -iname "*.wav" | while read -r file; do
        filename=$(basename "$file")
        base_name="${filename%.*}" 
        out_file="$OUT_DIR/$OUTPUT_DIR_MUSIC/$base_name.ogg"
        
        if [[ ! -f "$out_file" || "$file" -nt "$out_file" ]]; then
            echo "   -> Normalizzo e comprimo: $base_name.ogg"
            ffmpeg -y -hide_banner -loglevel error -i "$file" \
                   -af "loudnorm=I=-16:TP=-6:LRA=11" \
                   -c:a libvorbis -q:a 5 \
                   -ar 48000 \
                   "$out_file" </dev/null
        fi
    done
else
    # IL NUOVO AVVISO SE MANCA LA CARTELLA
    echo "⚠️  Attenzione: Cartella Musica ignorata."
    echo "   Non ho trovato la cartella '$RAW_DIR/$INPUT_DIR_MUSIC'."
    echo "   Crea questa cartella e mettici i file .wav per normalizzarli in .ogg."
fi

echo "✅ Pipeline completata! Asset pronti nel progetto."
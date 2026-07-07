#!/usr/bin/env bash
# Corregge i relPath dei tileset nel JSON LDtk dopo un Save/Export dall'editor.
# LDtk tende a riscrivere i path originali del bundle Franuka; questo script
# li allinea alla struttura del repository (tilesets/ e entities/).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
DEFAULT_LDtk="$REPO_ROOT/assets/maps/world/rpg-adventure-phase1.ldtk"

OLD_WORLD_PATH='Fantasy RPG asset pack/2x/RPG tileset (full) - 200%.png'
NEW_WORLD_PATH='tilesets/RPG tileset (full) - 200%.png'

OLD_HERO_PATH='Fantasy RPG heroes pack (by Franuka)/2x/Character sprites/Swashbuckler/04_Swashbuckler_Full Sheet (REFERENCE).png'
NEW_HERO_PATH='entities/04_Swashbuckler_Full Sheet (REFERENCE).png'

usage() {
    cat <<'EOF'
fix_ldtk_paths.sh — Allinea i path PNG nel file .ldtk alla cartella del repo

Uso:
  ./fix_ldtk_paths.sh [percorso/progetto.ldtk]

Se omesso, usa:
  assets/maps/world/rpg-adventure-phase1.ldtk

Struttura attesa accanto al .ldtk:
  tilesets/RPG tileset (full) - 200%.png
  entities/04_Swashbuckler_Full Sheet (REFERENCE).png

Esegui dopo ogni Save/Export da LDtk se i path tornano a quelli del bundle.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
fi

LDtk_FILE="${1:-$DEFAULT_LDtk}"

if [[ ! -f "$LDtk_FILE" ]]; then
    echo "Errore: file non trovato: $LDtk_FILE" >&2
    exit 1
fi

MAP_DIR="$(cd "$(dirname "$LDtk_FILE")" && pwd)"
LDtk_FILE="$MAP_DIR/$(basename "$LDtk_FILE")"

WORLD_PNG="$MAP_DIR/$NEW_WORLD_PATH"
HERO_PNG="$MAP_DIR/$NEW_HERO_PATH"

count_matches() {
    local pattern="$1"
    grep -Fc "$pattern" "$LDtk_FILE" || true
}

WORLD_BEFORE=$(count_matches "$OLD_WORLD_PATH")
HERO_BEFORE=$(count_matches "$OLD_HERO_PATH")

if [[ "$WORLD_BEFORE" -eq 0 && "$HERO_BEFORE" -eq 0 ]]; then
    if grep -Fq "$NEW_WORLD_PATH" "$LDtk_FILE" && grep -Fq "$NEW_HERO_PATH" "$LDtk_FILE"; then
        echo "Nessuna modifica necessaria: i path sono già corretti."
    else
        echo "Attenzione: path bundle non trovati e path repo non riconosciuti."
        echo "Verifica manualmente relPath / __tilesetRelPath in:"
        echo "  $LDtk_FILE"
    fi
else
    TMP_FILE="$(mktemp)"
    sed \
        -e "s|$(printf '%s' "$OLD_WORLD_PATH" | sed 's/[&|]/\\&/g')|$NEW_WORLD_PATH|g" \
        -e "s|$(printf '%s' "$OLD_HERO_PATH" | sed 's/[&|]/\\&/g')|$NEW_HERO_PATH|g" \
        "$LDtk_FILE" > "$TMP_FILE"
    mv "$TMP_FILE" "$LDtk_FILE"
    echo "Path aggiornati in: $LDtk_FILE"
    echo "  world: $WORLD_BEFORE occorrenza/e"
    echo "  hero:  $HERO_BEFORE occorrenza/e"
fi

missing=0
if [[ ! -f "$WORLD_PNG" ]]; then
    echo "Errore: PNG tileset mancante: $WORLD_PNG" >&2
    missing=1
fi
if [[ ! -f "$HERO_PNG" ]]; then
    echo "Errore: PNG hero mancante: $HERO_PNG" >&2
    missing=1
fi

if [[ "$missing" -ne 0 ]]; then
    exit 1
fi

echo "Verifica PNG: OK"

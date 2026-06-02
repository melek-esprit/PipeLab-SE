#!/usr/bin/env bash
# Génère un gros fichier texte de N Mo pour tester le traitement intensif
SIZE_MB=${1:-20}
OUT=${2:-data/big.txt}
mkdir -p "$(dirname "$OUT")"
> "$OUT"
LINE="The quick brown fox jumps over the lazy dog. 0123456789 ABCDEFGHIJ"
BYTES=$((SIZE_MB*1024*1024))

# Génération rapide : flux infini de lignes coupé à la taille voulue.
# (beaucoup plus rapide qu'un echo par ligne)
yes "$LINE" | head -c "$BYTES" > "$OUT"
# Garantit que le fichier se termine par une ligne complète.
echo "$LINE" >> "$OUT"

echo "Généré : $OUT  ($(wc -c < "$OUT") octets, $(wc -l < "$OUT") lignes)"

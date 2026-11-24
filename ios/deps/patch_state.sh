#!/bin/bash
# Correctifs d'un arbre git (gl4es) : appliqués si l'arbre est propre, vérifiés
# sinon. Sort en erreur si l'arbre est modifié autrement que par le correctif.
#   ios/deps/patch_state.sh <arbre git> <correctif>
# La comparaison ignore les lignes « index » : leurs empreintes abrégées
# dépendent de core.abbrev et de la taille du dépôt (7 ou 8 caractères ou plus),
# si bien qu'un correctif appliqué ne se comparait pas octet pour octet à son
# fichier et qu'une deuxième cible (sim après device) échouait.
set -euo pipefail
TREE="$1" PATCH="$2"
if git -C "$TREE" diff --quiet; then
  git -C "$TREE" apply "$PATCH"
elif ! cmp -s <(git -C "$TREE" diff | grep -v '^index ') <(grep -v '^index ' "$PATCH"); then
  echo "$TREE modifié et différent de $(basename "$PATCH") : à réconcilier" >&2; exit 1
fi

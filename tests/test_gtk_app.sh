#!/bin/sh
set -eu
binary="${1:-build/cpu-reader-gtk}"
version="$("$binary" --version)"
case "$version" in
  "CPU Reader "*) ;;
  *) echo "Unexpected --version output: $version" >&2; exit 1 ;;
esac
help="$("$binary" --help)"
case "$help" in
  *"Uso: cpu-reader"*) ;;
  *) echo "Unexpected --help output: $help" >&2; exit 1 ;;
esac
echo "GTK application CLI smoke tests passed."

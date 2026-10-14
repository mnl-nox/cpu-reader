#!/bin/sh
set -eu
binary="${1:-build/cpu-reader-gtk}"
if ! command -v xvfb-run >/dev/null 2>&1 || ! command -v timeout >/dev/null 2>&1; then
  echo "xvfb-run and timeout are required for the GTK UI smoke test." >&2
  exit 2
fi
set +e
GSK_RENDERER=cairo timeout 5s xvfb-run -a "$binary"
status=$?
set -e
if [ "$status" -ne 124 ]; then
  echo "GTK application exited before timeout (status $status)." >&2
  exit 1
fi
echo "GTK window startup smoke test passed."

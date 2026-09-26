#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT/android"
if [[ -x ./gradlew ]]; then
  ./gradlew assembleDebug
  ./gradlew assembleRelease bundleRelease
elif command -v gradle >/dev/null 2>&1; then
  gradle assembleDebug
  gradle assembleRelease bundleRelease
else
  echo "Gradle is required (or commit the Gradle wrapper)." >&2
  exit 2
fi
echo "Artifacts:"
find app/build/outputs -type f \( -name '*.apk' -o -name '*.aab' \) -print

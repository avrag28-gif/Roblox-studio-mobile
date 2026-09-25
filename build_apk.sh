#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
if [[ ! -d "$ROOT/android" ]]; then echo "android project missing"; exit 1; fi
cd "$ROOT/android"
if [[ -x ./gradlew ]]; then ./gradlew assembleDebug; else echo "Gradle wrapper not committed; use a provisioned Gradle installation"; exit 2; fi

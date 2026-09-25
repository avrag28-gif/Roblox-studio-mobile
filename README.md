# Roblox Studio Mobile

A mobile-first Roblox-inspired game editor and native C++ engine foundation for Android ARM64.

## Current milestone

Foundation / Phase 1: native engine core, Android shell, CMake build, and GitHub Actions APK pipeline.

## Goals

- Roblox-style DataModel / Instance hierarchy.
- Luau-compatible scripting layer.
- Touch-first 3D editor.
- Physics, assets, client/server runtime and versioning.
- Roblox publishing through supported public APIs.
- Android APK/AAB builds.

## Architecture

- src/core — DataModel, Instance, Signals and primitive engine types.
- android — Android application shell and native bridge.
- assets — runtime/editor assets.
- tests — engine tests.

Roblox compatibility is implemented through documented/public interfaces; proprietary Roblox internals are not reproduced.

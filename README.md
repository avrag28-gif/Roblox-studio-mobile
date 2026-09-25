# Roblox Studio Mobile
Native C++20 engine/editor foundation targeting Android ARM64 first, with an architecture that can later host iOS.

## Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure

The Android module is a native-shell scaffold and CI produces a debug APK. Production Vulkan/OpenGL ES rendering, full Luau embedding, real physics, device profiling and Roblox publishing are intentionally represented by replaceable interfaces until their external APIs are integrated and verified. No Roblox proprietary internals or credentials are included.

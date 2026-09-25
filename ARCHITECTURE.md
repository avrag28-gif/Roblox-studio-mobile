# Roblox Studio Mobile — implementation architecture
This repository is built as an independent Roblox-inspired editor/runtime. It does not reproduce proprietary Roblox internals.

## Layers
core -> math -> scene -> renderer/physics/input -> scripting/assets/audio/animation -> editor -> runtime -> networking -> platform.

## Invariants
1. Instance ownership is unique.
2. Parenting transfers ownership only through an owning unique_ptr API.
3. Runtime must not mutate the editor source scene.
4. Rendering and physics consume scene state through public engine interfaces.
5. Script execution is behind a capability boundary.
6. Project data must remain versioned and migratable.
7. Credentials never belong in source control.

## Mobile strategy
Android ARM64 is the first target. Renderer backends remain abstract so Vulkan can be added with an OpenGL ES-compatible fallback. Low-end devices use reduced render resolution, culling, batching and asset budgets.

## Roblox integration
The compatibility surface is intentionally limited to documented/public behavior. Publishing/authentication is an integration layer and must use the Roblox APIs and authorization model documented at implementation time.

# Architecture
The engine uses a typed Instance hierarchy and a scene/DataModel object graph. Rendering, physics, scripting, assets, audio, animation and networking are separate subsystems with explicit runtime/editor boundaries. Mobile platform code is isolated from portable C++.
## Compatibility
Roblox-like behavior is implemented only through documented/public interfaces; proprietary Roblox internals are not reproduced.
## Completion gates
Every subsystem requires unit tests, integration tests, build verification and runtime verification before release.
#pragma once
#include "../scene/scene_runtime.h"
#include "../physics/physics_engine.h"
#include "../scripting/luau_service.h"
#include "../assets/asset_pipeline.h"
#include "../audio/audio_mixer.h"
#include "../animation/animation_runtime.h"
#include "../security/sandbox.h"
#include "../runtime/runtime_system.h"
#include "../performance/performance_budget.h"
namespace rsm {
class EngineSystem{
 public:
  PhysicsEngine physics;LuauService scripts;AssetPipeline assets;AudioMixer audio;Animator animation;ScriptSandbox sandbox;RuntimeSystem runtime;FrameProfiler profiler;
};
}
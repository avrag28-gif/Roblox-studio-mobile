#pragma once
#include "../scene/scene.h"
#include "../renderer/renderer.h"
#include "../physics/physics_world.h"
#include "../performance/performance_runtime.h"
namespace rsm {
class Engine {
 bool initialized_=false;
 Scene scene_;PhysicsWorld physics_;PerformanceRuntime performance_;std::unique_ptr<Renderer>renderer_;
public:
 Engine():renderer_(std::make_unique<NullRenderer>()){}
 bool Initialize(){initialized_=renderer_->Initialize();return initialized_;}
 void Shutdown(){if(renderer_)renderer_->Shutdown();initialized_=false;physics_.Clear();}
 void Tick(float dt){if(!initialized_)return;performance_.BeginFrame();physics_.Step(dt,scene_.Game());performance_.Set("physics",0);renderer_->Render(scene_.Game());performance_.EndFrame();}
 bool Initialized()const{return initialized_;}
 Scene&EditorScene(){return scene_;}const Renderer&RendererBackend()const{return *renderer_;}PerformanceRuntime&Performance(){return performance_;}
};
}
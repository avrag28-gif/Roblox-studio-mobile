#pragma once
#include "../scene/scene.h"
#include "../renderer/renderer.h"
#include "../physics/physics_world.h"
namespace rsm { class Engine{public:Engine():renderer_(std::make_unique<NullRenderer>()){}bool Initialize(){return renderer_->Initialize();}void Tick(float dt){physics_.Step(dt,scene_.Game());renderer_->Render(scene_.Game());}Scene& EditorScene(){return scene_;}const Renderer& RendererBackend()const{return *renderer_;}private:Scene scene_;PhysicsWorld physics_;std::unique_ptr<Renderer> renderer_;};}

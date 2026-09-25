#pragma once
#include "../scene/scene.h"
#include "../physics/physics_world.h"
namespace rsm { class Engine { bool running_=false; Scene scene_; PhysicsWorld physics_; public: void Start(){running_=true;} void Stop(){running_=false;} bool Running()const{return running_;} void Tick(float dt){if(running_)physics_.Step(dt);} Scene& EditorScene(){return scene_;} PhysicsWorld& Physics(){return physics_;} }; }
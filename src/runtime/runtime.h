#pragma once
#include "../scene/scene.h"
#include "../physics/physics_world.h"
namespace rsm {class Runtime{Scene scene_;PhysicsWorld physics_;bool running_=false;public:bool Start(const Scene&source){if(!source.Game().Clone())return false;scene_.Reset();running_=true;return true;}void Step(float dt){if(running_)physics_.Step(dt,scene_.Game());}void Stop(){running_=false;}bool Running()const{return running_;}Scene&Game(){return scene_;}const Scene&Game()const{return scene_;}};}
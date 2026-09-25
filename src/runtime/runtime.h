#pragma once
#include "../scene/scene.h"
#include "../physics/physics_world.h"
namespace rsm { class Runtime{public:bool Start(const Scene& source){auto c=source.Game().Clone();if(!c)return false;running_=true;return true;}void Step(float dt){if(running_)physics_.Step(dt,scene_.Game());}void Stop(){running_=false;}bool Running()const{return running_;}private:Scene scene_;PhysicsWorld physics_;bool running_=false;};}

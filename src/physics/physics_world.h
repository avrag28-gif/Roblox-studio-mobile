#pragma once
#include "../core/base_part.h"
namespace rsm { class PhysicsWorld{public:explicit PhysicsWorld(Vector3 g={0,-196.2f,0}):gravity_(g){}void Step(float dt,const Instance& root){for(auto* x:root.GetDescendants()){auto* p=dynamic_cast<BasePart*>(x);if(!p||p->Anchored())continue;auto v=p->Position()+gravity_*dt;if(v.y<0)v.y=0;p->SetPosition(v);}}Vector3 Gravity()const{return gravity_;}private:Vector3 gravity_;};}

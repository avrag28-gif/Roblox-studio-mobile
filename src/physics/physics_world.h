#pragma once
#include "../math/vector3.h"
#include <cstddef>
#include <unordered_map>
namespace rsm { struct RigidBody { Vector3 position{},velocity{}; float mass=1; bool anchored=false,canCollide=true; }; class PhysicsWorld { Vector3 gravity_{0,-196.2f,0}; std::unordered_map<std::size_t,RigidBody> bodies_; public: void SetGravity(Vector3 g){gravity_=g;} Vector3 Gravity()const{return gravity_;} void Add(std::size_t id,RigidBody b){bodies_[id]=b;} void Remove(std::size_t id){bodies_.erase(id);} void Step(float dt){if(dt<=0)return;for(auto&[id,b]:bodies_){(void)id;if(!b.anchored){b.velocity=b.velocity+gravity_*dt;b.position=b.position+b.velocity*dt;}}} const RigidBody* Get(std::size_t id)const{auto i=bodies_.find(id);return i==bodies_.end()?nullptr:&i->second;} }; }
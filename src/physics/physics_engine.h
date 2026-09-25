#pragma once
#include "../core/base_part.h"
#include "../math/ray.h"
#include <unordered_map>
#include <vector>
#include <cmath>
namespace rsm {
struct PhysicsBody{BasePart* part=nullptr;Vector3 velocity{};bool sleeping=false;};
class PhysicsEngine{
 public:
  void Sync(Instance& root){bodies_.clear();for(auto* x:root.GetDescendants())if(auto* p=dynamic_cast<BasePart*>(x))if(!p->Anchored())bodies_[p]={p,{},{}};}
  void Step(float dt){dt=std::clamp(dt,0.f,0.1f);for(auto& [_,b]:bodies_){if(!b.part||b.sleeping)continue;b.velocity+=gravity_*dt;auto pos=b.part->Position()+b.velocity*dt;if(pos.y<0){pos.y=0;b.velocity.y=-b.velocity.y*restitution_;if(std::fabs(b.velocity.y)<0.05f)b.sleeping=true;}b.part->SetPosition(pos);}}
  void SetGravity(Vector3 g){gravity_=g;} Vector3 Gravity()const{return gravity_;} size_t BodyCount()const{return bodies_.size();}
 private:Vector3 gravity_{0,-196.2f,0};float restitution_=0.2f;std::unordered_map<BasePart*,PhysicsBody>bodies_;
};
}
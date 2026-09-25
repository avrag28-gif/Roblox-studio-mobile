#pragma once
#include "../core/base_part.h"
#include "../math/aabb.h"
#include <unordered_map>
namespace rsm {
struct RaycastHit{BasePart*part=nullptr;float distance=0;Vector3 position{};};
class PhysicsWorld{
  Vector3 gravity_;
  std::unordered_map<const BasePart*,Vector3> velocity_;
 public:
  explicit PhysicsWorld(Vector3 g={0,-196.2f,0}):gravity_(g){}
  void Step(float dt,const Instance&root){
    if(dt<=0)return;
    for(auto*x:root.GetDescendants()) if(auto*p=dynamic_cast<BasePart*>(x)){
      if(p->Anchored()){velocity_.erase(p);continue;}
      auto&v=velocity_[p];
      v+=gravity_*dt;
      auto pos=p->Position()+v*dt;
      const auto half=p->Size()*0.5f;
      if(pos.y-half.y<0){pos.y=half.y;if(v.y<0)v.y=0;}
      p->SetPosition(pos);
    }
  }
  RaycastHit Raycast(const Instance&root,const Ray&r)const{
    RaycastHit best;best.distance=1e30f;
    for(auto*x:root.GetDescendants()) if(auto*p=dynamic_cast<BasePart*>(x)) if(p->CanQuery()){
      auto h=p->Size()*0.5f; AABB b{p->Position()-h,p->Position()+h}; float t;
      if(b.IntersectRay(r,t)&&t>=0&&t<best.distance)best={p,t,r.At(t)};
    }
    if(!best.part)best.distance=0; return best;
  }
  Vector3 Gravity()const{return gravity_;}
  void Clear(){velocity_.clear();}
};
}
#pragma once
#include "../core/base_part.h"
#include "../math/aabb.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
namespace rsm {
struct RaycastHit{BasePart*part=nullptr;float distance=0;Vector3 position{};};
class PhysicsWorld{
 Vector3 gravity_;std::unordered_map<const BasePart*,Vector3> velocity_;float restitution_=.05f,friction_=.8f;
 static AABB Bounds(const BasePart*p,Vector3 pos){auto h=p->Size()*.5f;return{pos-h,pos+h};}
public:
 explicit PhysicsWorld(Vector3 g={0,-196.2f,0}):gravity_(g){}
 void SetRestitution(float v){restitution_=std::clamp(v,0.f,1.f);}void SetFriction(float v){friction_=std::clamp(v,0.f,1.f);}
 void Step(float dt,const Instance&root){
  if(dt<=0)return;std::vector<BasePart*> bodies;
  for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<BasePart*>(x)){bodies.push_back(p);if(p->Anchored())velocity_.erase(p);else velocity_[p]+=gravity_*dt;}
  for(auto*p:bodies)if(!p->Anchored()){auto&v=velocity_[p];auto pos=p->Position()+v*dt;auto half=p->Size()*.5f;
   if(pos.y-half.y<0){pos.y=half.y;if(v.y<0)v.y=-v.y*restitution_;v.x*=friction_;v.z*=friction_;}
   p->SetPosition(pos);
  }
  for(size_t i=0;i<bodies.size();++i)for(size_t j=i+1;j<bodies.size();++j){auto*a=bodies[i];auto*b=bodies[j];if(!a->CanCollide()||!b->CanCollide())continue;if(!Bounds(a,a->Position()).Intersects(Bounds(b,b->Position())))continue;
    if(a->Anchored()&&!b->Anchored()){auto&v=velocity_[b];v.y=std::max(0.f,v.y);auto bp=b->Position();bp.y=a->Position().y+a->Size().y*.5f+b->Size().y*.5f;b->SetPosition(bp);}
    else if(b->Anchored()&&!a->Anchored()){auto&v=velocity_[a];v.y=std::max(0.f,v.y);auto ap=a->Position();ap.y=b->Position().y+b->Size().y*.5f+a->Size().y*.5f;a->SetPosition(ap);}
  }
 }
 RaycastHit Raycast(const Instance&root,const Ray&r)const{RaycastHit best;best.distance=1e30f;for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<BasePart*>(x))if(p->CanQuery()){auto h=p->Size()*.5f;AABB b{p->Position()-h,p->Position()+h};float t;if(b.IntersectRay(r,t)&&t>=0&&t<best.distance)best={p,t,r.At(t)};}if(!best.part)best.distance=0;return best;}
 bool Overlap(const Instance&root,const AABB&q)const{for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<BasePart*>(x))if(p->CanQuery()&&Bounds(p,p->Position()).Intersects(q))return true;return false;}
 Vector3 Gravity()const{return gravity_;}void Clear(){velocity_.clear();}
};
}
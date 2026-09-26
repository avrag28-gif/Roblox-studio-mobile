#pragma once
#include "../core/data_model.h"
#include "../core/instance.h"
#include "../core/base_part.h"
#include "../renderer/camera.h"
#include "../physics/raycast.h"
#include <algorithm>
#include <cmath>
#include <utility>
namespace rsm {
class SelectionService {
 Instance* selected_=nullptr;
public:
 void Clear(){selected_=nullptr;}
 void Select(Instance* i){selected_=i;}
 Instance* Selected()const{return selected_;}
 Instance* RayPick(const DataModel& game,const Ray& ray) {
  float best=1e30f; Instance* hit=nullptr;
  for(auto* i:game.GetDescendants()) if(auto* p=dynamic_cast<BasePart*>(i)) {
   Vector3 min=p->Position()-p->Size()*0.5f, maxv=p->Position()+p->Size()*0.5f;
   float tmin=0,tmax=1e30f;
   const float o[3]={ray.origin.x,ray.origin.y,ray.origin.z}, d[3]={ray.direction.x,ray.direction.y,ray.direction.z};
   const float mn[3]={min.x,min.y,min.z}, mx[3]={maxv.x,maxv.y,maxv.z};
   bool ok=true;
   for(int a=0;a<3;a++){
    if(std::abs(d[a])<1e-6f){if(o[a]<mn[a]||o[a]>mx[a]){ok=false;break;}}
    else{float t1=(mn[a]-o[a])/d[a],t2=(mx[a]-o[a])/d[a];if(t1>t2)std::swap(t1,t2);tmin=std::max(tmin,t1);tmax=std::min(tmax,t2);if(tmin>tmax){ok=false;break;}}
   }
   if(ok&&tmin<best){best=tmin;hit=p;}
  }
  selected_=hit;return hit;
 }
};
}
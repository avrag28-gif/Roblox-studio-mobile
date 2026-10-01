#pragma once
#include "../core/data_model.h"
#include "../core/instance.h"
#include "../core/base_part.h"
#include "../physics/physics_world.h"
#include "../renderer/camera.h"
namespace rsm {
class SelectionService {
 Instance* selected_=nullptr;
public:
 void Clear(){selected_=nullptr;}
 void Select(Instance* i){selected_=i;}
 Instance* Selected()const{return selected_;}
 Instance* RayPick(const DataModel& game,const Ray& ray) {
  auto hit=PhysicsWorld().Raycast(game,ray);
  selected_=hit.part;
  return selected_;
 }
};
}

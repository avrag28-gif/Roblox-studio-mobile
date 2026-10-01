#pragma once
#include "../core/data_model.h"
#include "../core/instance.h"
#include "../core/base_part.h"
#include "../physics/physics_world.h"
#include <algorithm>
#include <vector>
namespace rsm {
class SelectionService {
 std::vector<Instance*> selected_;
public:
 void Clear(){selected_.clear();}
 void Select(Instance* i){selected_.clear();if(i)selected_.push_back(i);}
 void Add(Instance* i){if(!i)return;if(std::find(selected_.begin(),selected_.end(),i)==selected_.end())selected_.push_back(i);}
 void Remove(Instance* i){selected_.erase(std::remove(selected_.begin(),selected_.end(),i),selected_.end());}
 void Toggle(Instance* i){if(!i)return;auto it=std::find(selected_.begin(),selected_.end(),i);if(it==selected_.end())selected_.push_back(i);else selected_.erase(it);}
 Instance* Selected()const{return selected_.empty()?nullptr:selected_.front();}
 const std::vector<Instance*>&SelectedAll()const{return selected_;}
 bool Contains(const Instance*i)const{return std::find(selected_.begin(),selected_.end(),i)!=selected_.end();}
 Instance*RayPick(const DataModel&game,const Ray&ray){
  auto hit=PhysicsWorld().Raycast(game,ray);
  if(hit.part)Select(hit.part);else Clear();
  return Selected();
 }
};
}

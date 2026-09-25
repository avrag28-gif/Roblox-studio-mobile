#pragma once
#include "../math/ray.h"
#include "../core/instance.h"
namespace rsm {struct RaycastHit{Instance* instance=nullptr;float distance=0;};class Raycaster{public:RaycastHit Cast(const Ray&,const Instance&)const{return{};}};}

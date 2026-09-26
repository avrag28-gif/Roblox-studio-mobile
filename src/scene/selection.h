#pragma once
#include "../core/instance.h"
namespace rsm {
class SelectionModel { Instance* selected_=nullptr; public: void Set(Instance*i){selected_=i;} void Clear(){selected_=nullptr;} Instance* Get()const{return selected_;} };
}

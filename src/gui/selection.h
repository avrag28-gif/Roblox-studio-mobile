#pragma once
#include "../core/instance.h"
namespace rsm {class Selection{public:void Set(Instance* x){selected_=x;}Instance* Get()const{return selected_;}void Clear(){selected_=nullptr;}private:Instance* selected_=nullptr;};}

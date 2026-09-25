#pragma once
#include "../core/instance.h"
#include <cstddef>
namespace rsm{struct RenderObject{const Instance*instance=nullptr;};class RenderWorld{public:void Build(const Instance&root){count_=0;for(auto*x:root.GetDescendants())if(x)++count_;}std::size_t ObjectCount()const{return count_;}private:std::size_t count_=0;};}
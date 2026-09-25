#pragma once
#include "../core/base_part.h"
#include "../math/aabb.h"
#include <vector>
namespace rsm {struct RenderObject{const BasePart*part=nullptr;AABB bounds;};class RenderWorld{std::vector<RenderObject>objects_;public:void Build(const Instance&root){objects_.clear();for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<const BasePart*>(x)){auto h=p->Size()*0.5f;objects_.push_back({p,{p->Position()-h,p->Position()+h}});}}const std::vector<RenderObject>&Objects()const{return objects_;}std::size_t VisibleCount()const{return objects_.size();}};}
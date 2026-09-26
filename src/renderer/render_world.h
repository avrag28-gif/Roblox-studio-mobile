#pragma once
#include "../core/base_part.h"
#include "../math/aabb.h"
#include "../math/geometry.h"
#include <vector>
#include <algorithm>
namespace rsm {
struct RenderObject{const BasePart*part=nullptr;AABB bounds;};
class RenderWorld{
 std::vector<RenderObject>objects_,visible_;
public:
 void Build(const Instance&root){objects_.clear();for(auto*x:root.GetDescendants())if(auto*p=dynamic_cast<const BasePart*>(x)){auto h=p->Size()*.5f;objects_.push_back({p,{p->Position()-h,p->Position()+h}});}}
 void Cull(const std::vector<Plane>&planes){visible_.clear();for(const auto&o:objects_){bool v=true;for(const auto&p:planes)if(p.normal.Dot(o.bounds.max)+p.distance<0){v=false;break;}if(v)visible_.push_back(o);}}
 const std::vector<RenderObject>&Objects()const{return objects_;}
 const std::vector<RenderObject>&VisibleObjects()const{return visible_.empty()?objects_:visible_;}
 std::size_t VisibleCount()const{return VisibleObjects().size();}
};
}
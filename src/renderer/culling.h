#pragma once
#include "../math/math.h"
namespace rsm { inline bool Visible(const AABB& box,const Plane& p){return p.normal.Dot(box.max)+p.distance>=0;} }
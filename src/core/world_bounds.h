#pragma once
#include "base_part.h"
#include "transform_hierarchy.h"
#include "../math/aabb.h"
#include <algorithm>
#include <cmath>

namespace rsm {

inline AABB WorldBounds(const BasePart& part) {
    const auto q = WorldRotation(part).Normalized();
    const float axx = 1 - 2*(q.y*q.y + q.z*q.z);
    const float axy = 2*(q.x*q.y - q.z*q.w);
    const float axz = 2*(q.x*q.z + q.y*q.w);
    const float ayx = 2*(q.x*q.y + q.z*q.w);
    const float ayy = 1 - 2*(q.x*q.x + q.z*q.z);
    const float ayz = 2*(q.y*q.z - q.x*q.w);
    const float azx = 2*(q.x*q.z - q.y*q.w);
    const float azy = 2*(q.y*q.z + q.x*q.w);
    const float azz = 1 - 2*(q.x*q.x + q.y*q.y);
    const auto& s=part.Size();
    const Vector3 h{
        .5f*(std::fabs(axx)*s.x+std::fabs(axy)*s.y+std::fabs(axz)*s.z),
        .5f*(std::fabs(ayx)*s.x+std::fabs(ayy)*s.y+std::fabs(ayz)*s.z),
        .5f*(std::fabs(azx)*s.x+std::fabs(azy)*s.y+std::fabs(azz)*s.z)
    };
    const auto p=WorldCFrame(part).position;
    return {p-h,p+h};
}

}

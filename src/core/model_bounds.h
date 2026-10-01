#pragma once
#include "base_part.h"
#include "class_system.h"
#include "transform_hierarchy.h"
#include <algorithm>
#include <cmath>

namespace rsm {

inline Vector3 ModelWorldAabbSize(const Model& model) {
    bool hasPart = false;
    Vector3 minCorner{0, 0, 0};
    Vector3 maxCorner{0, 0, 0};

    for (auto* node : model.GetDescendants()) {
        auto* part = dynamic_cast<BasePart*>(node);
        if (!part) continue;

        const auto cf = WorldCFrame(*part);
        const auto& q = cf.rotation;
        const float axx = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
        const float axy = 2.0f * (q.x*q.y - q.z*q.w);
        const float axz = 2.0f * (q.x*q.z + q.y*q.w);
        const float ayx = 2.0f * (q.x*q.y + q.z*q.w);
        const float ayy = 1.0f - 2.0f * (q.x*q.x + q.z*q.z);
        const float ayz = 2.0f * (q.y*q.z - q.x*q.w);
        const float azx = 2.0f * (q.x*q.z - q.y*q.w);
        const float azy = 2.0f * (q.y*q.z + q.x*q.w);
        const float azz = 1.0f - 2.0f * (q.x*q.x + q.y*q.y);

        const auto& s = part->Size();
        const Vector3 half{
            0.5f * (std::fabs(axx)*s.x + std::fabs(ayx)*s.y + std::fabs(azx)*s.z),
            0.5f * (std::fabs(axy)*s.x + std::fabs(ayy)*s.y + std::fabs(azy)*s.z),
            0.5f * (std::fabs(axz)*s.x + std::fabs(ayz)*s.y + std::fabs(azz)*s.z)
        };
        const auto& p = part->Position();
        const Vector3 lo{p.x-half.x, p.y-half.y, p.z-half.z};
        const Vector3 hi{p.x+half.x, p.y+half.y, p.z+half.z};

        if (!hasPart) {
            minCorner = lo;
            maxCorner = hi;
            hasPart = true;
        } else {
            minCorner.x = std::min(minCorner.x, lo.x);
            minCorner.y = std::min(minCorner.y, lo.y);
            minCorner.z = std::min(minCorner.z, lo.z);
            maxCorner.x = std::max(maxCorner.x, hi.x);
            maxCorner.y = std::max(maxCorner.y, hi.y);
            maxCorner.z = std::max(maxCorner.z, hi.z);
        }
    }
    return hasPart ? maxCorner - minCorner : Vector3{0, 0, 0};
}

}

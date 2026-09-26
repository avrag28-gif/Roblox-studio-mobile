#pragma once
#include "vector3.h"
namespace rsm {
struct OBB{Vector3 center{},halfExtents{.5f,.5f,.5f};};
struct Plane{Vector3 normal{};float distance=0;float SignedDistance(Vector3 p)const{return normal.Dot(p)+distance;}};
}
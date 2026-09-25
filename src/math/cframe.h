#pragma once
#include "vector3.h"
#include "quaternion.h"
namespace rsm{struct CFrame{Vector3 position{};Quaternion rotation{};constexpr CFrame()=default;constexpr explicit CFrame(Vector3 p):position(p){}CFrame(Vector3 p,Quaternion q):position(p),rotation(q){}static CFrame Identity(){return{};}};}
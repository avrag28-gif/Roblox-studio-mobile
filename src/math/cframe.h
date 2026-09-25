#pragma once
#include "vector3.h"
namespace rsm { struct CFrame{Vector3 position{};constexpr CFrame()=default;constexpr explicit CFrame(Vector3 p):position(p){}static CFrame Identity(){return{};}}; }

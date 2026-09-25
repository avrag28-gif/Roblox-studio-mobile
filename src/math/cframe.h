#pragma once
#include "vector3.h"
namespace rsm { struct CFrame { Vector3 position{}; constexpr CFrame()=default; explicit constexpr CFrame(Vector3 p):position(p){} static constexpr CFrame Identity(){return{};} }; }
#pragma once
#include "../math/vector3.h"
#include "../math/ray.h"
#include <cmath>
namespace rsm {struct Camera{Vector3 position{0,5,10},target{};float fov=60,nearPlane=.05f,farPlane=2000;Ray ScreenRay(float x,float y,float w,float h)const{Vector3 f=(target-position).Normalized(),up{0,1,0};Vector3 right=f.Cross(up).Normalized();up=right.Cross(f).Normalized();float a=w/h,sy=std::tan(fov*3.14159265f/360.f),sx=sy*a;return {position,(f+right*((2*x/w-1)*sx)+up*((1-2*y/h)*sy)).Normalized()};}};}
#pragma once
#include "../math/vector3.h"
#include "../math/ray.h"
#include <algorithm>
#include <cmath>
namespace rsm {
struct Camera {
 Vector3 position{0,5,10}, target{};
 float fov=60, nearPlane=.05f, farPlane=2000;
 void Orbit(float yaw,float pitch){Vector3 d=position-target;float r=d.Length();if(r<.01f)r=10;float a=std::atan2(d.z,d.x)+yaw;float p=std::asin(std::clamp(d.y/r,-.999f,.999f))+pitch;p=std::clamp(p,-1.5f,1.5f);position={target.x+r*std::cos(p)*std::cos(a),target.y+r*std::sin(p),target.z+r*std::cos(p)*std::sin(a)};}
 void Zoom(float delta){Vector3 d=position-target;float r=std::max(1.f,d.Length()+delta);d=d.Normalized()*r;position=target+d;}
 Ray ScreenRay(float x,float y,float w,float h)const{if(h<=0)h=1;Vector3 f=(target-position).Normalized(),up{0,1,0};Vector3 right=Vector3::cross(f,up).Normalized();up=Vector3::cross(right,f).Normalized();float a=w/h,sy=std::tan(fov*3.14159265f/360.f),sx=sy*a;return {position,(f+right*((2*x/w-1)*sx)+up*((1-2*y/h)*sy)).Normalized()};}
};
}
#pragma once
#include <cmath>
namespace rsm{
struct Quaternion{
 float x=0,y=0,z=0,w=1;
 static Quaternion Identity(){return{};}
 static Quaternion FromAxisAngle(const struct Vector3&axis,float a){float s=std::sin(a*.5f);return{axis.x*s,axis.y*s,axis.z*s,std::cos(a*.5f)};}
 Quaternion Normalized()const{float n=std::sqrt(x*x+y*y+z*z+w*w);return n>0?Quaternion{x/n,y/n,z/n,w/n}:Quaternion{};}
 Quaternion operator*(const Quaternion&o)const{return{w*o.x+x*o.w+y*o.z-z*o.y,w*o.y-x*o.z+y*o.w+z*o.x,w*o.z+x*o.y-y*o.x+z*o.w,w*o.w-x*o.x-y*o.y-z*o.z};}
};
}
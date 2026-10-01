#pragma once
#include "vector3.h"
#include <cmath>
namespace rsm{
struct Quaternion{
 float x=0,y=0,z=0,w=1;
 static Quaternion Identity(){return{};}
 static Quaternion FromAxisAngle(const Vector3&axis,float a){float s=std::sin(a*.5f);return{axis.x*s,axis.y*s,axis.z*s,std::cos(a*.5f)};}
 Quaternion Normalized()const{float n=std::sqrt(x*x+y*y+z*z+w*w);return n>0?Quaternion{x/n,y/n,z/n,w/n}:Quaternion{};}
 Vector3 Rotate(Vector3 v)const{Quaternion q=Normalized();Quaternion p{v.x,v.y,v.z,0};Quaternion qi{-q.x,-q.y,-q.z,q.w};Quaternion r=q*p*qi;return{r.x,r.y,r.z};}
 Quaternion operator*(const Quaternion&o)const{return{w*o.x+x*o.w+y*o.z-z*o.y,w*o.y-x*o.z+y*o.w+z*o.x,w*o.z+x*o.y-y*o.x+z*o.w,w*o.w-x*o.x-y*o.y-z*o.z};}
};
}
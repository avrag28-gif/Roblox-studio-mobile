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
 static Quaternion FromEulerXYZ(float x,float y,float z){auto qx=FromAxisAngle({1,0,0},x),qy=FromAxisAngle({0,1,0},y),qz=FromAxisAngle({0,0,1},z);return(qz*qy*qx).Normalized();}
 Vector3 ToEulerXYZ()const{auto q=Normalized();float sinr=2*(q.w*q.x+q.y*q.z),cosr=1-2*(q.x*q.x+q.y*q.y);float x=std::atan2(sinr,cosr);float sinp=2*(q.w*q.y-q.z*q.x);float y=std::abs(sinp)>=1?std::copysign(3.14159265358979323846f/2,sinp):std::asin(sinp);float siny=2*(q.w*q.z+q.x*q.y),cosy=1-2*(q.y*q.y+q.z*q.z);float z=std::atan2(siny,cosy);return{x,y,z};}
 Quaternion operator*(const Quaternion&o)const{return{w*o.x+x*o.w+y*o.z-z*o.y,w*o.y-x*o.z+y*o.w+z*o.x,w*o.z+x*o.y-y*o.x+z*o.w,w*o.w-x*o.x-y*o.y-z*o.z};}
};
}
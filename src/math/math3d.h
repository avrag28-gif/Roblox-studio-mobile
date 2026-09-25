#pragma once
#include <cmath>
#include <algorithm>
namespace rsm {
struct Vector2{float x=0,y=0;};
struct Vector3{float x=0,y=0,z=0; constexpr Vector3()=default; constexpr Vector3(float X,float Y,float Z):x(X),y(Y),z(Z){} Vector3 operator+(Vector3 b)const{return{x+b.x,y+b.y,z+b.z};} Vector3 operator-(Vector3 b)const{return{x-b.x,y-b.y,z-b.z};} Vector3 operator*(float s)const{return{x*s,y*s,z*s};} Vector3& operator+=(Vector3 b){x+=b.x;y+=b.y;z+=b.z;return *this;} float length()const{return std::sqrt(x*x+y*y+z*z);} Vector3 normalized()const{float l=length();return l>1e-6f?*this*(1.f/l):Vector3{};} static float dot(Vector3 a,Vector3 b){return a.x*b.x+a.y*b.y+a.z*b.z;} static Vector3 cross(Vector3 a,Vector3 b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}};
struct Color3{float r=1,g=1,b=1;};
struct Quaternion{float x=0,y=0,z=0,w=1;};
struct CFrame{Vector3 position{}; Quaternion rotation{};};
struct Ray{Vector3 origin{},direction{0,0,-1}; Vector3 At(float t)const{return origin+direction*t;}};
struct AABB{Vector3 min{},max{}; bool Contains(Vector3 p)const{return p.x>=min.x&&p.x<=max.x&&p.y>=min.y&&p.y<=max.y&&p.z>=min.z&&p.z<=max.z;}};
}
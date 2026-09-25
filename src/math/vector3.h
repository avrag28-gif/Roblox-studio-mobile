#pragma once
#include <cmath>
namespace rsm { struct Vector3 { float x=0,y=0,z=0; constexpr Vector3()=default; constexpr Vector3(float X,float Y,float Z):x(X),y(Y),z(Z){} Vector3 operator+(Vector3 b)const{return{x+b.x,y+b.y,z+b.z};} Vector3 operator-(Vector3 b)const{return{x-b.x,y-b.y,z-b.z};} Vector3 operator*(float s)const{return{x*s,y*s,z*s};} float Dot(Vector3 b)const{return x*b.x+y*b.y+z*b.z;} float Length()const{return std::sqrt(Dot(*this));} Vector3 Normalized()const{float l=Length();return l>0?*this*(1.0f/l):Vector3{};} }; }

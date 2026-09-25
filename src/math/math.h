#pragma once
#include "vector3.h"
#include <algorithm>
namespace rsm {
struct Vector2{float x=0,y=0;};
struct Vector4{float x=0,y=0,z=0,w=0;};
struct Quaternion{float x=0,y=0,z=0,w=1;};
struct Matrix4{float m[16]{}; static Matrix4 Identity(){Matrix4 a{};a.m[0]=a.m[5]=a.m[10]=a.m[15]=1;return a;}};
struct Ray{Vector3 origin{},direction{0,0,-1};};
struct AABB{Vector3 min{},max{}; bool Contains(Vector3 p)const{return p.x>=min.x&&p.x<=max.x&&p.y>=min.y&&p.y<=max.y&&p.z>=min.z&&p.z<=max.z;}};
struct Plane{Vector3 normal{0,1,0};float distance=0;};
}
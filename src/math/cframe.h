#pragma once
#include "vector3.h"
#include "quaternion.h"
#include "matrix4.h"
namespace rsm{struct CFrame{Vector3 position{};Quaternion rotation{};constexpr CFrame()=default;constexpr explicit CFrame(Vector3 p):position(p){}CFrame(Vector3 p,Quaternion q):position(p),rotation(q){}static CFrame Identity(){return{};}Matrix4 Matrix()const{auto q=rotation.Normalized();Matrix4 r=Matrix4::Identity();float xx=q.x*q.x,yy=q.y*q.y,zz=q.z*q.z,xy=q.x*q.y,xz=q.x*q.z,yz=q.y*q.z,wx=q.w*q.x,wy=q.w*q.y,wz=q.w*q.z;r.m[0]=1-2*(yy+zz);r.m[1]=2*(xy-wz);r.m[2]=2*(xz+wy);r.m[4]=2*(xy+wz);r.m[5]=1-2*(xx+zz);r.m[6]=2*(yz-wx);r.m[8]=2*(xz-wy);r.m[9]=2*(yz+wx);r.m[10]=1-2*(xx+yy);r.m[12]=position.x;r.m[13]=position.y;r.m[14]=position.z;return r;}};}
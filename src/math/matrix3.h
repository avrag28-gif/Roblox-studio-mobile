#pragma once
#include "vector3.h"
namespace rsm {struct Matrix3{float m[9]{};static Matrix3 Identity(){Matrix3 x;x.m[0]=x.m[4]=x.m[8]=1;return x;}Vector3 operator*(const Vector3&v)const{return {m[0]*v.x+m[1]*v.y+m[2]*v.z,m[3]*v.x+m[4]*v.y+m[5]*v.z,m[6]*v.x+m[7]*v.y+m[8]*v.z};}};}
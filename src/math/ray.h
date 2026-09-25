#pragma once
#include "vector3.h"
namespace rsm { struct Ray{Vector3 origin{},direction{0,0,-1};Vector3 At(float t)const{return origin+direction*t;}}; }

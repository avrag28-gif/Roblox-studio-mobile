#pragma once
#include <cmath>
namespace rsm{struct Quaternion{float x=0,y=0,z=0,w=1;static Quaternion Identity(){return{};}Quaternion Normalized()const{float n=std::sqrt(x*x+y*y+z*z+w*w);return n>0?Quaternion{x/n,y/n,z/n,w/n}:Quaternion{};}};}
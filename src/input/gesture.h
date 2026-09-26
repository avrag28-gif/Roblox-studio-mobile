#pragma once
#include "touch.h"
#include <cmath>
namespace rsm {
class GestureRecognizer {
 float lastDistance_=0,lastAngle_=0;
public:
 float PinchScale(float a,float b){if(b<=0)return 1;float s=a/b;lastDistance_=b;return s;}
 float RotationDelta(float ax,float ay,float bx,float by){float a=std::atan2(ay,ax),b=std::atan2(by,bx);float d=b-a;while(d>3.14159265f)d-=6.2831853f;while(d<-3.14159265f)d+=6.2831853f;lastAngle_=b;return d;}
 float LastDistance()const{return lastDistance_;}float LastAngle()const{return lastAngle_;}
};
}
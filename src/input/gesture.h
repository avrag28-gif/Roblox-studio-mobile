#pragma once
#include "input_state.h"
#include <cmath>
namespace rsm { class GestureRecognizer{float lastDistance_=0;public:float PinchScale(float a,float b){if(b<=0)return 1;float s=a/b;lastDistance_=b;return s;}float LastDistance()const{return lastDistance_;}};}
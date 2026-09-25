#pragma once
#include "../math/vector3.h"
namespace rsm {class CameraController{public:void Orbit(float dx,float dy){yaw_+=dx;pitch_+=dy;if(pitch_>89)pitch_=89;if(pitch_<-89)pitch_=-89;}void Zoom(float d){distance_+=d;if(distance_<1)distance_=1;}float Yaw()const{return yaw_;}float Pitch()const{return pitch_;}float Distance()const{return distance_;}private:float yaw_=0,pitch_=25,distance_=10;};}

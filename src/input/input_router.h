#pragma once
#include "gesture.h"
#include "../editor/gizmo_controller.h"
namespace rsm {
struct TouchPoint{int id=0;float x=0,y=0;};
class InputRouter {
 GestureState gesture_; bool mouseMode_=false;
public:
 void SetDesktopMode(bool v){mouseMode_=v;}
 bool DesktopMode()const{return mouseMode_;}
 GestureState& Gesture(){return gesture_;}
 Vector2 Pinch(float a,float b,float c,float d)const{return {std::sqrt((c-a)*(c-a)+(d-b)*(d-b)),0};}
};}
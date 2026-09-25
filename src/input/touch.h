#pragma once
namespace rsm{enum class TouchType{Down,Move,Up};struct TouchEvent{int id=0;float x=0,y=0;TouchType type=TouchType::Down;};struct GestureState{int fingers=0;float pinch=1,rotation=0;};}
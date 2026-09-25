#pragma once
namespace rsm { enum class TouchPhase{Down,Move,Up,Cancel};struct TouchEvent{int pointerId=0;float x=0,y=0,dx=0,dy=0;TouchPhase phase=TouchPhase::Down;};}

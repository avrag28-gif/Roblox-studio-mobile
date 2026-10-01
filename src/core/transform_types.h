#pragma once
#include <cmath>
#include <algorithm>
namespace rsm {
enum class TransformMode { Move, Rotate, Scale };
enum class TransformSpace { World, Local };
enum class GizmoAxis { X, Y, Z, XYZ, None };
enum class GizmoMode { Move, Rotate, Scale };
struct TransformSnapSettings {
 bool enabled=false;
 float move=1.0f;
 float rotate=15.0f;
 float scale=0.5f;
};
inline float SnapScalar(float v,float step,bool enabled){
 if(!enabled||step<=0)return v;
 return std::round(v/step)*step;
}
}

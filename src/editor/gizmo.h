#pragma once
#include "../math/vector3.h"
namespace rsm { enum class GizmoMode{Move,Rotate,Scale}; enum class GizmoAxis{X,Y,Z,XYZ}; struct Gizmo{GizmoMode mode=GizmoMode::Move;GizmoAxis axis=GizmoAxis::XYZ;Vector3 value{};};}
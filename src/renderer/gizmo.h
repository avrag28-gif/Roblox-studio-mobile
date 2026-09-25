#pragma once
namespace rsm { enum class GizmoMode{Move,Rotate,Scale};enum class GizmoAxis{X,Y,Z,XYZ};struct GizmoState{GizmoMode mode=GizmoMode::Move;GizmoAxis axis=GizmoAxis::XYZ;};}

#pragma once
#include "../math/vector3.h"
#include "../core/transform_types.h"
namespace rsm { struct Gizmo{GizmoMode mode=GizmoMode::Move;GizmoAxis axis=GizmoAxis::XYZ;Vector3 value{};};}
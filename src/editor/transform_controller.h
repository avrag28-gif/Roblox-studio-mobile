#pragma once
#include "../core/base_part.h"
#include "../core/transform_hierarchy.h"
#include <algorithm>

namespace rsm {

class TransformController {
public:
    void Begin(BasePart* p, TransformMode m, GizmoAxis a, TransformSpace s=TransformSpace::World) {
        target_=p; mode_=m; axis_=a; space_=s;
        start_=p ? WorldCFrame(*p) : CFrame{};
    }

    void Apply(float dx, float dy) {
        if (!target_) return;
        const float q=(dx-dy)*.01f;
        if (mode_==TransformMode::Move) {
            Vector3 d{};
            if (axis_==GizmoAxis::X || axis_==GizmoAxis::XYZ) d.x+=dx;
            if (axis_==GizmoAxis::Y || axis_==GizmoAxis::XYZ) d.y-=dy;
            if (axis_==GizmoAxis::Z || axis_==GizmoAxis::XYZ) d.z+=dy;
            if (space_==TransformSpace::Local) d=WorldRotation(*target_).Rotate(d);
            SetWorldCFrame(*target_, CFrame(WorldPosition(*target_)+d, WorldRotation(*target_)));
        } else if (mode_==TransformMode::Scale) {
            auto s=target_->Size();
            if (axis_==GizmoAxis::X || axis_==GizmoAxis::XYZ) s.x=std::max(.05f,s.x+q);
            if (axis_==GizmoAxis::Y || axis_==GizmoAxis::XYZ) s.y=std::max(.05f,s.y+q);
            if (axis_==GizmoAxis::Z || axis_==GizmoAxis::XYZ) s.z=std::max(.05f,s.z+q);
            target_->SetSize(s);
        } else {
            Quaternion delta;
            if (axis_==GizmoAxis::X) delta=Quaternion::FromAxisAngle({1,0,0},q);
            else if (axis_==GizmoAxis::Y) delta=Quaternion::FromAxisAngle({0,1,0},q);
            else delta=Quaternion::FromAxisAngle({0,0,1},q);
            if (space_==TransformSpace::Local) RotateLocal(*target_,delta);
            else RotateWorld(*target_,delta);
        }
    }

    void End(){target_=nullptr;}
    const CFrame& Start() const { return start_; }
private:
    BasePart* target_=nullptr;
    TransformMode mode_=TransformMode::Move;
    GizmoAxis axis_=GizmoAxis::XYZ;
    TransformSpace space_=TransformSpace::World;
    CFrame start_{};
};

}

#pragma once
#include "../core/base_part.h"
#include "../core/class_system.h"
#include "../core/transform_hierarchy.h"
#include <algorithm>

namespace rsm {
class TransformController {
public:
    void Begin(Instance* target,TransformMode m,GizmoAxis a,TransformSpace s=TransformSpace::World){
        target_=target;mode_=m;axis_=a;space_=s;start_=target?WorldCFrame(*target):CFrame{};
    }
    void Begin(BasePart* p,TransformMode m,GizmoAxis a,TransformSpace s=TransformSpace::World){Begin(static_cast<Instance*>(p),m,a,s);}

    void Apply(float dx,float dy){
        if(!target_)return;
        const float q=(dx-dy)*.01f;
        if(mode_==TransformMode::Move){
            Vector3 d{};
            if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)d.x+=dx;
            if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)d.y-=dy;
            if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)d.z+=dy;
            if(space_==TransformSpace::Local)d=WorldRotation(*target_).Rotate(d);
            SetWorldCFrame(*target_,CFrame(WorldPosition(*target_)+d,WorldRotation(*target_)));
        } else if(mode_==TransformMode::Scale){
            if(auto* part=dynamic_cast<BasePart*>(target_)){
                auto s=part->Size();
                if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)s.x=std::max(.05f,s.x+q);
                if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)s.y=std::max(.05f,s.y+q);
                if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)s.z=std::max(.05f,s.z+q);
                part->SetSize(s);
            } else if(auto* model=dynamic_cast<Model*>(target_)){
                Vector3 factors{1,1,1};
                if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)factors.x=std::max(.001f,1+q);
                if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)factors.y=std::max(.001f,1+q);
                if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)factors.z=std::max(.001f,1+q);
                ScaleModel(*model,factors);
            }
        } else {
            Quaternion delta;
            if(axis_==GizmoAxis::X)delta=Quaternion::FromAxisAngle({1,0,0},q);
            else if(axis_==GizmoAxis::Y)delta=Quaternion::FromAxisAngle({0,1,0},q);
            else delta=Quaternion::FromAxisAngle({0,0,1},q);
            if(space_==TransformSpace::Local)RotateLocal(*target_,delta);
            else RotateWorld(*target_,delta);
        }
    }
    void End(){target_=nullptr;}
    const CFrame& Start()const{return start_;}
private:
    Instance* target_=nullptr;
    TransformMode mode_=TransformMode::Move;
    GizmoAxis axis_=GizmoAxis::XYZ;
    TransformSpace space_=TransformSpace::World;
    CFrame start_{};
};
}

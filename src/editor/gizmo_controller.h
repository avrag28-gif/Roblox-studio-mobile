#pragma once
#include "../core/base_part.h"
#include "../core/class_system.h"
#include "../core/transform_hierarchy.h"
#include <algorithm>

namespace rsm {
class GizmoController {
    GizmoMode mode_=GizmoMode::Move;
    GizmoAxis axis_=GizmoAxis::XYZ;
    TransformSpace space_=TransformSpace::World;
public:
    void SetMode(GizmoMode m){mode_=m;}
    void SetAxis(GizmoAxis a){axis_=a;}
    void SetSpace(TransformSpace s){space_=s;}
    GizmoMode Mode()const{return mode_;}
    GizmoAxis Axis()const{return axis_;}
    TransformSpace Space()const{return space_;}

    void Apply(Instance& target,const Vector3& delta)const {
        Vector3 d=delta;
        if(axis_==GizmoAxis::X)d={delta.x,0,0};
        else if(axis_==GizmoAxis::Y)d={0,delta.y,0};
        else if(axis_==GizmoAxis::Z)d={0,0,delta.z};

        if(mode_==GizmoMode::Move) {
            if(space_==TransformSpace::Local)d=WorldRotation(target).Rotate(d);
            SetWorldCFrame(target,CFrame(WorldPosition(target)+d,WorldRotation(target)));
        } else if(mode_==GizmoMode::Scale) {
            if(auto* part=dynamic_cast<BasePart*>(&target)){
                auto s=part->Size()+d;
                part->SetSize({std::max(.05f,s.x),std::max(.05f,s.y),std::max(.05f,s.z)});
            } else if(auto* model=dynamic_cast<Model*>(&target)){
                Vector3 factors{1,1,1};
                if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)factors.x=std::max(.001f,1+d.x);
                if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)factors.y=std::max(.001f,1+d.y);
                if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)factors.z=std::max(.001f,1+d.z);
                ScaleModel(*model,factors);
            }
        } else {
            Quaternion q;
            if(axis_==GizmoAxis::X)q=Quaternion::FromAxisAngle({1,0,0},delta.x);
            else if(axis_==GizmoAxis::Y)q=Quaternion::FromAxisAngle({0,1,0},delta.y);
            else q=Quaternion::FromAxisAngle({0,0,1},delta.z);
            if(space_==TransformSpace::Local)RotateLocal(target,q);
            else RotateWorld(target,q);
        }
    }

    void Apply(BasePart& p,const Vector3& delta)const{Apply(static_cast<Instance&>(p),delta);}
};
}

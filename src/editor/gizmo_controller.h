#pragma once
#include "../core/base_part.h"
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

    void Apply(BasePart& p,const Vector3& delta)const {
        Vector3 d=delta;
        if(axis_==GizmoAxis::X)d={delta.x,0,0};
        else if(axis_==GizmoAxis::Y)d={0,delta.y,0};
        else if(axis_==GizmoAxis::Z)d={0,0,delta.z};

        if(mode_==GizmoMode::Move) {
            if(space_==TransformSpace::Local)d=WorldRotation(p).Rotate(d);
            SetWorldCFrame(p,CFrame(WorldPosition(p)+d,WorldRotation(p)));
        } else if(mode_==GizmoMode::Scale) {
            auto s=p.Size()+d;
            p.SetSize({std::max(.05f,s.x),std::max(.05f,s.y),std::max(.05f,s.z)});
        } else {
            const float angle=delta.Length();
            Quaternion q;
            if(axis_==GizmoAxis::X) q=Quaternion::FromAxisAngle({1,0,0},delta.x);
            else if(axis_==GizmoAxis::Y) q=Quaternion::FromAxisAngle({0,1,0},delta.y);
            else q=Quaternion::FromAxisAngle({0,0,1},delta.z);
            if(space_==TransformSpace::Local) RotateLocal(p,q);
            else RotateWorld(p,q);
            (void)angle;
        }
    }
};

}

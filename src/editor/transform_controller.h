#pragma once
#include "../core/base_part.h"
#include "../core/class_system.h"
#include "../core/transform_hierarchy.h"
#include <algorithm>
#include <cmath>
namespace rsm {
class TransformController {
 TransformSnapSettings snap_{};
public:
 void SetSnap(TransformSnapSettings s){snap_=s;}
 void SetSnapping(bool e){snap_.enabled=e;}
 const TransformSnapSettings&Snap()const{return snap_;}
 void Begin(Instance*target,TransformMode m,GizmoAxis a,TransformSpace s=TransformSpace::World){target_=target;mode_=m;axis_=a;space_=s;start_=target?WorldCFrame(*target):CFrame{};}
 void Begin(BasePart*p,TransformMode m,GizmoAxis a,TransformSpace s=TransformSpace::World){Begin(static_cast<Instance*>(p),m,a,s);}
 void Apply(float dx,float dy){
  if(!target_)return;
  if(mode_==TransformMode::Move){
   Vector3 d{};
   if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)d.x=dx;
   if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)d.y=-dy;
   if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)d.z=dy;
   if(space_==TransformSpace::Local)d=WorldRotation(*target_).Rotate(d);
   auto p=WorldPosition(*target_)+d;
   p.x=SnapScalar(p.x,snap_.move,snap_.enabled);p.y=SnapScalar(p.y,snap_.move,snap_.enabled);p.z=SnapScalar(p.z,snap_.move,snap_.enabled);
   SetWorldCFrame(*target_,CFrame(p,WorldRotation(*target_)));
  }else if(mode_==TransformMode::Scale){
   const float q=(dx-dy);
   if(auto*part=dynamic_cast<BasePart*>(target_)){
    auto s=part->Size();Vector3 d{};
    if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)d.x=q;
    if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)d.y=q;
    if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)d.z=q;
    if(space_==TransformSpace::World)d=Quaternion{-WorldRotation(*part).Normalized().x,-WorldRotation(*part).Normalized().y,-WorldRotation(*part).Normalized().z,WorldRotation(*part).Normalized().w}.Rotate(d);
    s+=d;s.x=std::max(.05f,SnapScalar(s.x,snap_.scale,snap_.enabled));s.y=std::max(.05f,SnapScalar(s.y,snap_.scale,snap_.enabled));s.z=std::max(.05f,SnapScalar(s.z,snap_.scale,snap_.enabled));part->SetSize(s);
   }else if(auto*model=dynamic_cast<Model*>(target_)){
    Vector3 f{1,1,1};if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)f.x=std::max(.001f,1+q*.01f);if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)f.y=std::max(.001f,1+q*.01f);if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)f.z=std::max(.001f,1+q*.01f);ScaleModel(*model,f);
   }
  }else{
   float degrees=(dx-dy);degrees=SnapScalar(degrees,snap_.rotate,snap_.enabled);float rad=degrees*3.14159265358979323846f/180.0f;
   Quaternion q;if(axis_==GizmoAxis::X)q=Quaternion::FromAxisAngle({1,0,0},rad);else if(axis_==GizmoAxis::Y)q=Quaternion::FromAxisAngle({0,1,0},rad);else q=Quaternion::FromAxisAngle({0,0,1},rad);
   if(space_==TransformSpace::Local)RotateLocal(*target_,q);else RotateWorld(*target_,q);
  }
 }
 void End(){target_=nullptr;}
 const CFrame&Start()const{return start_;}
private:
 Instance*target_=nullptr;TransformMode mode_=TransformMode::Move;GizmoAxis axis_=GizmoAxis::XYZ;TransformSpace space_=TransformSpace::World;CFrame start_{};
};
}

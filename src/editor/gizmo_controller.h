#pragma once
#include "../core/base_part.h"
#include "../core/class_system.h"
#include "../core/transform_hierarchy.h"
#include <algorithm>
#include <numeric>
#include <vector>
namespace rsm {
class GizmoController {
 GizmoMode mode_=GizmoMode::Move;
 GizmoAxis axis_=GizmoAxis::XYZ;
 TransformSpace space_=TransformSpace::World;
 TransformSnapSettings snap_{};
public:
 void SetMode(GizmoMode m){mode_=m;}
 void SetAxis(GizmoAxis a){axis_=a;}
 void SetSpace(TransformSpace s){space_=s;}
 void SetSnap(TransformSnapSettings s){snap_=s;}
 void SetSnapping(bool e){snap_.enabled=e;}
 const TransformSnapSettings&Snap()const{return snap_;}
 GizmoMode Mode()const{return mode_;}
 GizmoAxis Axis()const{return axis_;}
 TransformSpace Space()const{return space_;}

 void Apply(Instance&target,const Vector3&delta)const{
  Vector3 d=delta;
  if(axis_==GizmoAxis::X)d={delta.x,0,0};
  else if(axis_==GizmoAxis::Y)d={0,delta.y,0};
  else if(axis_==GizmoAxis::Z)d={0,0,delta.z};
  if(mode_==GizmoMode::Move){
   if(space_==TransformSpace::Local)d=WorldRotation(target).Rotate(d);
   auto p=WorldPosition(target)+d;
   p.x=SnapScalar(p.x,snap_.move,snap_.enabled);
   p.y=SnapScalar(p.y,snap_.move,snap_.enabled);
   p.z=SnapScalar(p.z,snap_.move,snap_.enabled);
   SetWorldCFrame(target,CFrame(p,WorldRotation(target)));
  }else if(mode_==GizmoMode::Scale){
   if(auto*part=dynamic_cast<BasePart*>(&target)){
    Vector3 local=d;
    if(space_==TransformSpace::World)local=Quaternion{-WorldRotation(target).Normalized().x,-WorldRotation(target).Normalized().y,-WorldRotation(target).Normalized().z,WorldRotation(target).Normalized().w}.Rotate(d);
    auto s=part->Size()+local;
    s.x=std::max(.05f,SnapScalar(s.x,snap_.scale,snap_.enabled));
    s.y=std::max(.05f,SnapScalar(s.y,snap_.scale,snap_.enabled));
    s.z=std::max(.05f,SnapScalar(s.z,snap_.scale,snap_.enabled));
    part->SetSize(s);
   }else if(auto*model=dynamic_cast<Model*>(&target)){
    Vector3 factors{1,1,1};
    if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)factors.x=std::max(.001f,1+d.x);
    if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)factors.y=std::max(.001f,1+d.y);
    if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)factors.z=std::max(.001f,1+d.z);
    ScaleModel(*model,factors);
   }
  }else{
   Quaternion q;
   if(axis_==GizmoAxis::X)q=Quaternion::FromAxisAngle({1,0,0},SnapScalar(delta.x,snap_.rotate,snap_.enabled)*3.14159265358979323846f/180.0f);
   else if(axis_==GizmoAxis::Y)q=Quaternion::FromAxisAngle({0,1,0},SnapScalar(delta.y,snap_.rotate,snap_.enabled)*3.14159265358979323846f/180.0f);
   else q=Quaternion::FromAxisAngle({0,0,1},SnapScalar(delta.z,snap_.rotate,snap_.enabled)*3.14159265358979323846f/180.0f);
   if(space_==TransformSpace::Local)RotateLocal(target,q);else RotateWorld(target,q);
  }
 }
 void ApplyGroup(const std::vector<Instance*>&targets,const Vector3&delta)const{
  if(targets.empty())return;
  Vector3 pivot{};size_t count=0;
  for(auto*i:targets)if(i){pivot+=WorldPosition(*i);++count;}
  if(!count)return;pivot=pivot*(1.0f/static_cast<float>(count));
  Vector3 d=delta;
  if(axis_==GizmoAxis::X)d={delta.x,0,0};else if(axis_==GizmoAxis::Y)d={0,delta.y,0};else if(axis_==GizmoAxis::Z)d={0,0,delta.z};
  if(mode_==GizmoMode::Move){
   if(space_==TransformSpace::Local){
    if(auto*i=targets.front())d=WorldRotation(*i).Rotate(d);
   }
   pivot+=d;pivot.x=SnapScalar(pivot.x,snap_.move,snap_.enabled);pivot.y=SnapScalar(pivot.y,snap_.move,snap_.enabled);pivot.z=SnapScalar(pivot.z,snap_.move,snap_.enabled);
   const Vector3 applied=pivot-(std::accumulate(targets.begin(),targets.end(),Vector3{},[](Vector3 a,Instance*i){return i?a+WorldPosition(*i):a;})*(1.0f/static_cast<float>(count)));
   for(auto*i:targets)if(i)SetWorldCFrame(*i,CFrame(WorldPosition(*i)+applied,WorldRotation(*i)));
  }else if(mode_==GizmoMode::Rotate){
   float degrees=axis_==GizmoAxis::X?delta.x:(axis_==GizmoAxis::Y?delta.y:delta.z);
   degrees=SnapScalar(degrees,snap_.rotate,snap_.enabled);
   const Vector3 axis=axis_==GizmoAxis::X?Vector3{1,0,0}:(axis_==GizmoAxis::Y?Vector3{0,1,0}:Vector3{0,0,1});
   Quaternion q=Quaternion::FromAxisAngle(axis,degrees*3.14159265358979323846f/180.0f);
   for(auto*i:targets)if(i){auto cf=WorldCFrame(*i);cf.position=pivot+q.Rotate(cf.position-pivot);cf.rotation=(space_==TransformSpace::Local?(cf.rotation*q):(q*cf.rotation)).Normalized();SetWorldCFrame(*i,cf);}
  }else{
   Vector3 factors{1,1,1};if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)factors.x=std::max(.001f,1+delta.x);if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)factors.y=std::max(.001f,1+delta.y);if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)factors.z=std::max(.001f,1+delta.z);
   for(auto*i:targets)if(i){auto cf=WorldCFrame(*i);auto local=pivot.Inverse().PointToWorldSpace(cf.position);auto scaled=Vector3{local.x*factors.x,local.y*factors.y,local.z*factors.z};cf.position=pivot.PointToWorldSpace(scaled);if(auto*part=dynamic_cast<BasePart*>(i)){auto s=part->Size();s.x=std::max(.05f,SnapScalar(s.x*factors.x,snap_.scale,snap_.enabled));s.y=std::max(.05f,SnapScalar(s.y*factors.y,snap_.scale,snap_.enabled));s.z=std::max(.05f,SnapScalar(s.z*factors.z,snap_.scale,snap_.enabled));part->SetSize(s);}else if(auto*model=dynamic_cast<Model*>(i))ScaleModel(*model,factors);SetWorldCFrame(*i,cf);}
  }
 }
 void Apply(BasePart&p,const Vector3&delta)const{Apply(static_cast<Instance&>(p),delta);}
};
}

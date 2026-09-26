#pragma once
#include "../core/base_part.h"
#include "../math/vector3.h"
namespace rsm {
enum class GizmoMode { Move, Rotate, Scale };
enum class GizmoAxis { X,Y,Z,XYZ };
class GizmoController {
 GizmoMode mode_=GizmoMode::Move; GizmoAxis axis_=GizmoAxis::XYZ;
public:
 void SetMode(GizmoMode m){mode_=m;} void SetAxis(GizmoAxis a){axis_=a;}
 GizmoMode Mode()const{return mode_;} GizmoAxis Axis()const{return axis_;}
 void Apply(BasePart& p,const Vector3&delta)const{
   Vector3 d=delta; if(axis_==GizmoAxis::X)d={delta.x,0,0}; if(axis_==GizmoAxis::Y)d={0,delta.y,0}; if(axis_==GizmoAxis::Z)d={0,0,delta.z};
   if(mode_==GizmoMode::Move)p.SetPosition(p.Position()+d);
   else if(mode_==GizmoMode::Scale){auto s=p.Size()+d;p.SetSize({std::max(.05f,s.x),std::max(.05f,s.y),std::max(.05f,s.z)});}
   else {auto c=p.CFrameValue();c.rotation={c.rotation.x+d.x,c.rotation.y+d.y,c.rotation.z+d.z};p.SetCFrame(c);}
 }
};}
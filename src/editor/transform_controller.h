#pragma once
#include "../core/base_part.h"
namespace rsm {
enum class GizmoAxis{X,Y,Z,XYZ};
enum class TransformMode{Move,Rotate,Scale};
class TransformController {
public:
 void Begin(BasePart* p,TransformMode m,GizmoAxis a){part_=p;mode_=m;axis_=a;start_=p?p->CFrameValue():CFrame{};}
 void Apply(float dx,float dy){
  if(!part_)return;
  if(mode_==TransformMode::Move){auto v=part_->Position();if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)v.x+=dx;if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)v.y-=dy;if(axis_==GizmoAxis::Z)v.z+=dy;part_->SetPosition(v);}
  else if(mode_==TransformMode::Scale){auto s=part_->Size();float q=(dx-dy)*.01f;if(axis_==GizmoAxis::X||axis_==GizmoAxis::XYZ)s.x=std::max(.1f,s.x+q);if(axis_==GizmoAxis::Y||axis_==GizmoAxis::XYZ)s.y=std::max(.1f,s.y+q);if(axis_==GizmoAxis::Z||axis_==GizmoAxis::XYZ)s.z=std::max(.1f,s.z+q);part_->SetSize(s);}
  else {auto c=part_->CFrameValue();float a=(dx-dy)*.01f;Quaternion q=axis_==GizmoAxis::X?Quaternion::FromAxisAngle({1,0,0},a):axis_==GizmoAxis::Y?Quaternion::FromAxisAngle({0,1,0},a):Quaternion::FromAxisAngle({0,0,1},a);c.rotation=q*c.rotation;part_->SetCFrame(c);}
 }
 void End(){part_=nullptr;}
private:BasePart*part_=nullptr;TransformMode mode_=TransformMode::Move;GizmoAxis axis_=GizmoAxis::XYZ;CFrame start_{};
};
}
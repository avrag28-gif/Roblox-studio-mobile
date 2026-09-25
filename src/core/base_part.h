#pragma once
#include "instance.h"
#include "../math/vector3.h"
#include "../math/cframe.h"
#include "../math/color3.h"
namespace rsm {
enum class PartShape{Block,Sphere,Cylinder,Wedge,CornerWedge};
class BasePart:public Instance{
public:
 explicit BasePart(std::string className="BasePart"):Instance(std::move(className)){}
 const Vector3& Position()const{return position_;} void SetPosition(Vector3 v){position_=v;PropertyChanged.Fire("Position");}
 const CFrame& CFrameValue()const{return cframe_;} void SetCFrame(CFrame v){cframe_=v;position_=v.position;PropertyChanged.Fire("CFrame");PropertyChanged.Fire("Position");}
 const Vector3& Size()const{return size_;} void SetSize(Vector3 v){size_=v;PropertyChanged.Fire("Size");}
 const Color3& Color()const{return color_;} void SetColor(Color3 v){color_=v;PropertyChanged.Fire("Color");}
 bool Anchored()const{return anchored_;} void SetAnchored(bool v){if(anchored_==v)return;anchored_=v;PropertyChanged.Fire("Anchored");}
 bool CanCollide()const{return canCollide_;} void SetCanCollide(bool v){if(canCollide_==v)return;canCollide_=v;PropertyChanged.Fire("CanCollide");}
 PartShape Shape()const{return shape_;} void SetShape(PartShape v){shape_=v;PropertyChanged.Fire("Shape");}
 std::unique_ptr<Instance> Clone()const override{auto c=std::make_unique<BasePart>(ClassName());c->SetName(Name());c->SetCFrame(cframe_);c->SetSize(size_);c->SetColor(color_);c->SetAnchored(anchored_);c->SetCanCollide(canCollide_);c->SetShape(shape_);return c;}
private: Vector3 position_{};CFrame cframe_{};Vector3 size_{1,1,1};Color3 color_{};bool anchored_=false,canCollide_=true;PartShape shape_=PartShape::Block;
};
class Part:public BasePart{public:Part():BasePart("Part"){} std::unique_ptr<Instance> Clone()const override{auto c=std::make_unique<Part>();c->SetName(Name());c->SetCFrame(CFrameValue());c->SetSize(Size());c->SetColor(Color());c->SetAnchored(Anchored());c->SetCanCollide(CanCollide());c->SetShape(Shape());return c;}};
}

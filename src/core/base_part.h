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
 const Vector3& Size()const{return size_;} void SetSize(Vector3 v){size_=v;PropertyChanged.Fire("Size");}
 const Color3& Color()const{return color_;} void SetColor(Color3 v){color_=v;PropertyChanged.Fire("Color");}
 bool Anchored()const{return anchored_;} void SetAnchored(bool v){anchored_=v;PropertyChanged.Fire("Anchored");}
 bool CanCollide()const{return canCollide_;} void SetCanCollide(bool v){canCollide_=v;PropertyChanged.Fire("CanCollide");}
 PartShape Shape()const{return shape_;} void SetShape(PartShape v){shape_=v;PropertyChanged.Fire("Shape");}
 std::unique_ptr<Instance> Clone()const override{auto c=std::make_unique<BasePart>(ClassName());c->SetName(Name());c->SetSize(size_);c->SetPosition(position_);c->SetColor(color_);c->SetAnchored(anchored_);c->SetCanCollide(canCollide_);c->SetShape(shape_);return c;}
private: Vector3 position_{};Vector3 size_{1,1,1};Color3 color_{};bool anchored_=false,canCollide_=true;PartShape shape_=PartShape::Block;
};
class Part:public BasePart{public:Part():BasePart("Part"){}std::unique_ptr<Instance> Clone()const override{auto c=std::make_unique<Part>(*this);return std::unique_ptr<Instance>(new Part(*this));}};
}
#pragma once
#include "base_part.h"
#include "class_system.h"
#include "transform_hierarchy.h"
#include "property.h"
#include <string>
namespace rsm {
class PropertyAccess {
public:
 static bool Get(const Instance&i,const std::string&name,PropertyValue&out){
  if(name=="Name"){out=i.Name();return true;}
  if(name=="Archivable"){out=i.Archivable();return true;}
  if(auto*m=dynamic_cast<const Model*>(&i)){
   if(name=="Pivot"){out=m->Pivot();return true;}
   if(name=="CFrame"){out=WorldCFrame(i);return true;}
  }
  if(auto*p=dynamic_cast<const BasePart*>(&i)){
   if(name=="Position"){out=WorldPosition(i);return true;}
   if(name=="CFrame"){out=WorldCFrame(i);return true;}
   if(name=="Size"){out=p->Size();return true;}
   if(name=="Color"){out=p->Color();return true;}
   if(name=="Transparency"){out=(double)p->Transparency();return true;}
   if(name=="Anchored"){out=p->Anchored();return true;}
   if(name=="CanCollide"){out=p->CanCollide();return true;}
   if(name=="CanTouch"){out=p->CanTouch();return true;}
   if(name=="CanQuery"){out=p->CanQuery();return true;}
   if(name=="Mass"){out=(double)p->Mass();return true;}
  }
  return false;
 }
 static bool Set(Instance&i,const std::string&name,const PropertyValue&v){
  if(name=="Name"&&std::holds_alternative<std::string>(v)){i.SetName(std::get<std::string>(v));return true;}
  if(name=="Archivable"&&std::holds_alternative<bool>(v)){i.SetArchivable(std::get<bool>(v));return true;}
  if(auto*m=dynamic_cast<Model*>(&i)){
   if(name=="Pivot"&&std::holds_alternative<CFrame>(v)){SetWorldCFrame(i,std::get<CFrame>(v));return true;}
   if(name=="CFrame"&&std::holds_alternative<CFrame>(v)){SetWorldCFrame(i,std::get<CFrame>(v));return true;}
  }
  if(auto*p=dynamic_cast<BasePart*>(&i)){
   if(name=="Position"&&std::holds_alternative<Vector3>(v)){SetWorldCFrame(i,CFrame(std::get<Vector3>(v),WorldRotation(i)));return true;}
   if(name=="CFrame"&&std::holds_alternative<CFrame>(v)){SetWorldCFrame(i,std::get<CFrame>(v));return true;}
   if(name=="Size"&&std::holds_alternative<Vector3>(v)){p->SetSize(std::get<Vector3>(v));return true;}
   if(name=="Color"&&std::holds_alternative<Color3>(v)){p->SetColor(std::get<Color3>(v));return true;}
   if(name=="Transparency"&&std::holds_alternative<double>(v)){p->SetTransparency((float)std::get<double>(v));return true;}
   if(name=="Anchored"&&std::holds_alternative<bool>(v)){p->SetAnchored(std::get<bool>(v));return true;}
   if(name=="CanCollide"&&std::holds_alternative<bool>(v)){p->SetCanCollide(std::get<bool>(v));return true;}
   if(name=="CanTouch"&&std::holds_alternative<bool>(v)){p->SetCanTouch(std::get<bool>(v));return true;}
   if(name=="CanQuery"&&std::holds_alternative<bool>(v)){p->SetCanQuery(std::get<bool>(v));return true;}
   if(name=="Mass"&&std::holds_alternative<double>(v)){p->SetMass((float)std::get<double>(v));return true;}
  }
  return false;
 }
};
}

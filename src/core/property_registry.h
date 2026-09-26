#pragma once
#include "property.h"
#include "base_part.h"
#include <unordered_map>
namespace rsm {
class PropertyRegistry {
 std::unordered_map<std::string,std::vector<PropertyDescriptor>> byClass_;
public:
 static PropertyRegistry&Instance(){static PropertyRegistry r;return r;}
 void Register(std::string cls,PropertyDescriptor d){byClass_[std::move(cls)].push_back(std::move(d));}
 const std::vector<PropertyDescriptor>*For(const std::string&cls)const{
  auto i=byClass_.find(cls);return i==byClass_.end()?nullptr:&i->second;
 }
 void RegisterBuiltins(){
  if(byClass_.count("Part"))return;
  auto add=[this](const char*n,PropertyType t,PropertyValue def){
   PropertyDescriptor d{n,t,true,std::move(def)};Register("BasePart",d);Register("Part",d);Register("MeshPart",d);
  };
  add("Position",PropertyType::Vector3,Vector3{});add("CFrame",PropertyType::CFrame,CFrame{});
  add("Size",PropertyType::Vector3,Vector3{1,1,1});add("Color",PropertyType::Color3,Color3{});
  add("Transparency",PropertyType::Number,0.0);add("Anchored",PropertyType::Bool,false);
  add("CanCollide",PropertyType::Bool,true);add("CanTouch",PropertyType::Bool,true);
  add("CanQuery",PropertyType::Bool,true);add("Mass",PropertyType::Number,1.0);
  Register("Instance",PropertyDescriptor{"Name",PropertyType::String,true,{}});
  Register("Instance",PropertyDescriptor{"Archivable",PropertyType::Bool,true,{}});
 }
};
}
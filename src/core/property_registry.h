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
 const std::vector<PropertyDescriptor>*For(const std::string&cls)const{auto i=byClass_.find(cls);return i==byClass_.end()?nullptr:&i->second;}
 void RegisterBuiltins(){
  if(byClass_.count("Part"))return;
  auto add=[this](const char*n,PropertyType t){PropertyDescriptor d{n,t,true,{}};Register("BasePart",d);Register("Part",d);};
  add("Position",PropertyType::Vector3);add("Size",PropertyType::Vector3);add("Color",PropertyType::Color3);add("Transparency",PropertyType::Number);add("Anchored",PropertyType::Bool);add("CanCollide",PropertyType::Bool);add("CanTouch",PropertyType::Bool);add("CanQuery",PropertyType::Bool);add("Mass",PropertyType::Number);
 }
};
}
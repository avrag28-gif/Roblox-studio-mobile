#pragma once
#include "../math/vector3.h"
#include "../math/color3.h"
#include "../math/cframe.h"
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
namespace rsm {
using PropertyValue=std::variant<std::monostate,bool,int64_t,double,std::string,Vector3,Color3,CFrame>;
enum class PropertyType{Bool,Integer,Number,String,Vector3,Color3,CFrame,Unknown};
struct PropertyDescriptor{std::string name;PropertyType type=PropertyType::Unknown;bool writable=true;PropertyValue defaultValue{};};
class PropertyBag{
 std::unordered_map<std::string,PropertyValue> values_;
 std::unordered_map<std::string,PropertyDescriptor> desc_;
 std::function<void(const std::string&)> changed_;
 static bool TypeMatches(PropertyType t,const PropertyValue&v){
  switch(t){case PropertyType::Bool:return std::holds_alternative<bool>(v);case PropertyType::Integer:return std::holds_alternative<int64_t>(v);
  case PropertyType::Number:return std::holds_alternative<double>(v)||std::holds_alternative<int64_t>(v);case PropertyType::String:return std::holds_alternative<std::string>(v);
  case PropertyType::Vector3:return std::holds_alternative<Vector3>(v);case PropertyType::Color3:return std::holds_alternative<Color3>(v);
  case PropertyType::CFrame:return std::holds_alternative<CFrame>(v);default:return true;}}
 public:
 void Define(PropertyDescriptor d){values_[d.name]=d.defaultValue;desc_[d.name]=std::move(d);}
 bool Has(const std::string&n)const{return desc_.count(n)!=0;}
 const PropertyDescriptor*Describe(const std::string&n)const{auto i=desc_.find(n);return i==desc_.end()?nullptr:&i->second;}
 const PropertyValue*Get(const std::string&n)const{auto i=values_.find(n);return i==values_.end()?nullptr:&i->second;}
 bool Set(const std::string&n,PropertyValue v){auto d=desc_.find(n);if(d==desc_.end()||!d->second.writable||!TypeMatches(d->second.type,v))return false;values_[n]=std::move(v);if(changed_)changed_(n);return true;}
 void OnChanged(std::function<void(const std::string&)> cb){changed_=std::move(cb);}
 const auto&Descriptors()const{return desc_;}
};
}
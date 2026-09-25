#pragma once
#include "instance.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
namespace rsm { using PropertyValue=std::variant<std::monostate,bool,int64_t,double,std::string>; enum class PropertyType{Bool,Integer,Number,String,Unknown}; struct PropertyDescriptor{std::string name;PropertyType type=PropertyType::Unknown;bool writable=true;PropertyValue defaultValue{};}; class PropertyBag{std::unordered_map<std::string,PropertyValue> values_;std::unordered_map<std::string,PropertyDescriptor> desc_;public:void Define(PropertyDescriptor d){values_[d.name]=d.defaultValue;desc_[d.name]=std::move(d);}bool Has(const std::string&n)const{return desc_.count(n)!=0;}const PropertyDescriptor*Describe(const std::string&n)const{auto i=desc_.find(n);return i==desc_.end()?nullptr:&i->second;}const PropertyValue*Get(const std::string&n)const{auto i=values_.find(n);return i==values_.end()?nullptr:&i->second;}bool Set(const std::string&n,PropertyValue v){auto d=desc_.find(n);if(d==desc_.end()||!d->second.writable)return false;values_[n]=std::move(v);return true;}};}
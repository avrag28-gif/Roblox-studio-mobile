#pragma once
#include "signal.h"
#include <string>
#include <variant>
#include <unordered_map>
namespace rsm {
using PropertyValue=std::variant<bool,double,std::string>;
class PropertyBag{std::unordered_map<std::string,PropertyValue> values_;public:Signal<const std::string&> Changed;void Set(std::string n,PropertyValue v){auto i=values_.find(n);if(i!=values_.end()&&i->second==v)return;values_[n]=std::move(v);Changed.Fire(n);}const PropertyValue* Get(const std::string& n)const{auto i=values_.find(n);return i==values_.end()?nullptr:&i->second;}};
}
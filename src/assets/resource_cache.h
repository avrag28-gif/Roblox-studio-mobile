#pragma once
#include <memory>
#include <string>
#include <unordered_map>
namespace rsm {
template<class T> class ResourceCache {
 struct Entry{std::shared_ptr<T>value;uint64_t lastUse=0;};
 std::unordered_map<std::string,Entry>items_;size_t max_=256;
public:
 void Put(std::string key,std::shared_ptr<T>v,uint64_t tick){if(items_.size()>=max_)EvictOne();items_[std::move(key)]={std::move(v),tick};}
 std::shared_ptr<T>Get(const std::string&key,uint64_t tick){auto i=items_.find(key);if(i==items_.end())return{};i->second.lastUse=tick;return i->second.value;}
 void EvictOne(){if(items_.empty())return;auto victim=items_.begin();for(auto i=items_.begin();i!=items_.end();++i)if(i->second.lastUse<victim->second.lastUse)victim=i;items_.erase(victim);}
 void Clear(){items_.clear();}size_t Size()const{return items_.size();}
};
}
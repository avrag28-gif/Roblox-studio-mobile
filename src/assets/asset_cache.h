#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
namespace rsm {
struct AssetCacheEntry { std::string id; size_t bytes=0; uint64_t lastUse=0; };
class AssetCache {
 std::unordered_map<std::string,AssetCacheEntry> entries_; size_t budget_=256u*1024u*1024u, used_=0; uint64_t tick_=0;
 void Evict(){while(used_>budget_&&!entries_.empty()){auto it=entries_.begin();for(auto i=entries_.begin();i!=entries_.end();++i)if(i->second.lastUse<it->second.lastUse)it=i;used_-=it->second.bytes;entries_.erase(it);}}
public:
 explicit AssetCache(size_t budget=256u*1024u*1024u):budget_(budget){}
 bool Put(std::string id,size_t bytes){if(id.empty()||bytes>budget_)return false;auto i=entries_.find(id);if(i!=entries_.end()){used_-=i->second.bytes;i->second={id,bytes,++tick_};}else entries_.emplace(id,AssetCacheEntry{id,bytes,++tick_});used_+=bytes;Evict();return entries_.count(id)!=0;}
 bool Touch(const std::string&id){auto i=entries_.find(id);if(i==entries_.end())return false;i->second.lastUse=++tick_;return true;}
 bool Remove(const std::string&id){auto i=entries_.find(id);if(i==entries_.end())return false;used_-=i->second.bytes;entries_.erase(i);return true;}
 void Clear(){entries_.clear();used_=0;}
 size_t Used()const{return used_;} size_t Budget()const{return budget_;} size_t Size()const{return entries_.size();}
};
}
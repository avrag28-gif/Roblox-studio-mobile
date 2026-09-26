#pragma once
#include <cstdint>
#include <unordered_map>
namespace rsm {
class OwnershipTable{
 std::unordered_map<uint64_t,uint64_t> owners_;
public:
 bool Set(uint64_t object,uint64_t client){owners_[object]=client;return true;}
 void Clear(uint64_t object){owners_.erase(object);}
 uint64_t Owner(uint64_t object)const{auto i=owners_.find(object);return i==owners_.end()?0:i->second;}
 bool CanWrite(uint64_t object,uint64_t client)const{auto o=Owner(object);return o==0||o==client;}
};}
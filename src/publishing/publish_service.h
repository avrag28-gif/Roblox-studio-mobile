#pragma once
#include <cstdint>
#include <string>
namespace rsm {
struct PublishTarget{uint64_t universeId=0,placeId=0;};
class PublishService{
 public:
  bool Validate(PublishTarget t,std::string&error)const{if(!t.universeId||!t.placeId){error="universeId and placeId are required";return false;}error.clear();return true;}
  std::string Endpoint(PublishTarget t)const{return "https://apis.roblox.com/universes/v1/"+std::to_string(t.universeId)+"/places/"+std::to_string(t.placeId)+"/versions?versionType=Published";}
};
}
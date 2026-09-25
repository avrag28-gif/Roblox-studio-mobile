#pragma once
#include <cstdint>
#include <string>
namespace rsm {struct PublishRequest{uint64_t universeId=0,placeId=0;std::string filePath;};class RobloxOpenCloudPublisher{public:static constexpr const char*Endpoint="https://apis.roblox.com/universes/v1/{universeId}/places/{placeId}/versions?versionType=Published";bool Validate(const PublishRequest&r,std::string&e)const{if(!r.universeId||!r.placeId){e="universeId and placeId are required";return false;}if(r.filePath.empty()){e="place file is required";return false;}e.clear();return true;}};}
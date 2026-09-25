#pragma once
#include <string>
#include <functional>
namespace rsm {
struct PublishRequest{std::string universeId;std::string placeId;std::string projectPayload;std::string contentType="application/octet-stream";};
struct PublishResult{bool ok=false;int status=0;std::string message;};
class RobloxPublisher {
public:
 using Transport=std::function<PublishResult(const std::string&url,const std::string&apiKey,const PublishRequest&)>;
 explicit RobloxPublisher(Transport t={}):transport_(std::move(t)){}
 PublishResult Publish(const PublishRequest&r,const std::string&apiKey)const{
  if(r.universeId.empty()||r.placeId.empty()||r.projectPayload.empty())return{false,0,"missing publish parameters"};
  if(apiKey.empty())return{false,401,"authorization required"};
  if(!transport_)return{false,0,"no HTTPS transport configured"};
  return transport_("https://apis.roblox.com/universes/v1/"+r.universeId+"/places/"+r.placeId+"/versions?versionType=Published",apiKey,r);
 }
private:Transport transport_;
};
}
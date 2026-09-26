#pragma once
#include "../network/replication_runtime.h"
#include "../network/transport.h"
#include <string>
namespace rsm {
enum class RuntimeRole{Editor,Server,Client};
class RuntimeContext {
 RuntimeRole role_; LoopbackTransport* transport_=nullptr; ReplicationRuntime replication_;
public:
 explicit RuntimeContext(RuntimeRole r,LoopbackTransport* t=nullptr):role_(r),transport_(t){}
 RuntimeRole Role()const{return role_;}
 void Send(uint32_t channel,const std::vector<uint8_t>&payload){if(transport_)transport_->Send(channel,payload);}
 bool PublishProperty(const std::string&path,const std::string&property,const std::string&value){
  if(role_!=RuntimeRole::Server)return false; replication_.Queue(path,property,value); return true;
 }
 bool ConsumeProperty(const std::function<bool(const ReplicationMessage&)>&apply){
  ReplicationMessage m; if(!replication_.Pop(m))return false; return replication_.ApplyOrdered(m,apply);
 }
 LoopbackTransport*Transport()const{return transport_;}
 ReplicationRuntime&Replication(){return replication_;}
};
}
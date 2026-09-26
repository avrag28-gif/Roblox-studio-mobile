#pragma once
#include "../network/replication_runtime.h"
#include "../network/transport.h"
namespace rsm {
enum class RuntimeRole{Editor,Server,Client};
class RuntimeContext {
 RuntimeRole role_; LoopbackTransport* transport_=nullptr; ReplicationRuntime replication_;
public:
 explicit RuntimeContext(RuntimeRole r,LoopbackTransport* t=nullptr):role_(r),transport_(t){}
 RuntimeRole Role()const{return role_;}
 void Send(uint32_t channel,const std::vector<uint8_t>&payload){if(transport_)transport_->Send(channel,payload);}
 LoopbackTransport*Transport()const{return transport_;}
 ReplicationRuntime&Replication(){return replication_;}
};
}
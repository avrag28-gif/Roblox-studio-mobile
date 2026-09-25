#pragma once
#include "../core/instance.h"
#include <cstdint>
#include <functional>
#include <queue>
namespace rsm {
struct RpcMessage{uint32_t id=0;std::string method;std::string payload;};
class ReplicationChannel{
 public:
  uint32_t Send(std::string method,std::string payload){RpcMessage m{next_++,std::move(method),std::move(payload)};queue_.push(m);return m.id;}
  bool Receive(RpcMessage& out){if(queue_.empty())return false;out=queue_.front();queue_.pop();return true;}
  size_t Pending()const{return queue_.size();}
 private:uint32_t next_=1;std::queue<RpcMessage>queue_;
};
}
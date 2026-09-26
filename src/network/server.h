#pragma once
#include "transport.h"
#include <unordered_set>
namespace rsm {
class NetworkServer {
 LoopbackTransport transport_; std::unordered_set<uint64_t>clients_;
public:
 LoopbackTransport&Transport(){return transport_;}
 uint64_t Connect(){auto id=transport_.Send(0,{});clients_.insert(id);return id;}
 bool Disconnect(uint64_t id){return clients_.erase(id)>0;}
 size_t ClientCount()const{return clients_.size();}
 void Broadcast(uint32_t ch,const std::vector<uint8_t>&p){for(size_t i=0;i<clients_.size();++i)transport_.Send(ch,p);}
};}
#pragma once
#include <cstdint>
#include <deque>
#include <vector>
namespace rsm {
struct NetworkMessage {uint64_t sequence=0;uint32_t channel=0;std::vector<uint8_t> payload;};
class LoopbackTransport {
 uint64_t next_=1; std::deque<NetworkMessage> q_;
public:
 uint64_t Send(uint32_t channel,const std::vector<uint8_t>& payload){auto id=next_++;q_.push_back({id,channel,payload});return id;}
 bool Receive(NetworkMessage& m){if(q_.empty())return false;m=std::move(q_.front());q_.pop_front();return true;}
 size_t Pending()const{return q_.size();}
};
}
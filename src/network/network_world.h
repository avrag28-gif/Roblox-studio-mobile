#pragma once
#include <cstdint>
#include <queue>
#include <vector>
namespace rsm {
struct Packet{uint32_t channel=0;uint64_t sequence=0;std::vector<uint8_t>data;};
class NetworkWorld{
 std::queue<Packet>q_;uint64_t next_=1;
 public:
  Packet Build(uint32_t channel,std::vector<uint8_t> data){return Packet{channel,next_++,std::move(data)};}
  void Send(Packet p){q_.push(std::move(p));}
  bool Receive(Packet&p){if(q_.empty())return false;p=std::move(q_.front());q_.pop();return true;}
  std::size_t Pending()const{return q_.size();}
  void Clear(){std::queue<Packet> empty;std::swap(q_,empty);}
};
}
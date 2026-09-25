#pragma once
#include <cstdint>
#include <queue>
#include <string>
namespace rsm {struct Packet{std::uint32_t channel=0;std::string payload;};class NetworkWorld{public:void Send(Packet p){q_.push(std::move(p));}bool Poll(Packet& p){if(q_.empty())return false;p=std::move(q_.front());q_.pop();return true;}private:std::queue<Packet> q_;};}

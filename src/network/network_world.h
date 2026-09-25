#pragma once
#include <cstdint>
#include <deque>
#include <vector>
namespace rsm{struct Packet{std::uint32_t channel=0;std::vector<std::uint8_t>payload;};class NetworkWorld{public:void Send(Packet p){q_.push_back(std::move(p));}bool Receive(Packet&p){if(q_.empty())return false;p=std::move(q_.front());q_.pop_front();return true;}std::size_t Pending()const{return q_.size();}private:std::deque<Packet>q_;};}
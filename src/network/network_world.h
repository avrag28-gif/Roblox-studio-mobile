#pragma once
#include <cstdint>
#include <queue>
#include <vector>
namespace rsm {struct Packet{uint32_t channel=0;std::vector<uint8_t>data;};class NetworkWorld{std::queue<Packet>q;public:void Send(Packet p){q.push(std::move(p));}bool Receive(Packet&p){if(q.empty())return false;p=std::move(q.front());q.pop();return true;}std::size_t Pending()const{return q.size();}};}
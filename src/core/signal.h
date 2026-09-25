#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <cstddef>
namespace rsm {
template<class... Args> class Signal {
public:
 using Callback=std::function<void(Args...)>; using ConnectionId=std::size_t;
 ConnectionId Connect(Callback cb){std::lock_guard l(m_); auto id=next_++; slots_[id]=std::move(cb); return id;}
 void Disconnect(ConnectionId id){std::lock_guard l(m_); slots_.erase(id);}
 void Fire(Args... args){std::vector<Callback> s; {std::lock_guard l(m_); for(auto& [id,cb]:slots_) s.push_back(cb);} for(auto& cb:s) if(cb) cb(args...);}
private: std::mutex m_; std::unordered_map<ConnectionId,Callback> slots_; ConnectionId next_=1;
};
}

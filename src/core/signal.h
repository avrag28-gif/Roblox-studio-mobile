#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <utility>
namespace rsm {
template<class... A> class Signal {
public:
 using Callback=std::function<void(A...)>; using Id=std::size_t;
 class Connection {
   Signal* s_{}; Id id_{};
 public:
   Connection()=default; Connection(Signal* s,Id id):s_(s),id_(id){}
   Connection(Connection&& o) noexcept:s_(o.s_),id_(o.id_){o.s_=nullptr;}
   Connection& operator=(Connection&& o) noexcept { if(this!=&o){Disconnect();s_=o.s_;id_=o.id_;o.s_=nullptr;}return *this; }
   Connection(const Connection&)=delete; Connection& operator=(const Connection&)=delete;
   ~Connection(){Disconnect();}
   void Disconnect(){if(s_){s_->Disconnect(id_);s_=nullptr;}}
   Id IdValue() const noexcept{return id_;}
   explicit operator bool() const{return s_!=nullptr;}
 };
 Connection Connect(Callback cb){std::lock_guard<std::mutex> l(m_);Id id=next_++;slots_[id]=std::move(cb);return Connection(this,id);}
 void Disconnect(Id id){std::lock_guard<std::mutex> l(m_);slots_.erase(id);}
 void Disconnect(Connection& c){c.Disconnect();}
 void Clear(){std::lock_guard<std::mutex> l(m_);slots_.clear();}
 void Fire(A... a){std::vector<Callback> copy;{std::lock_guard<std::mutex> l(m_);for(auto& [_,cb]:slots_)copy.push_back(cb);}for(auto& cb:copy)if(cb)cb(a...);}
private:
 std::mutex m_; std::unordered_map<Id,Callback> slots_; Id next_=1;
};
}
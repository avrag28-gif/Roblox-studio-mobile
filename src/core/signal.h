#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <cstddef>
namespace rsm {template<class...A>class Signal{public:using Callback=std::function<void(A...)>;using Id=std::size_t;class Connection{Signal*s{};Id i{};public:Connection()=default;Connection(Signal*x,Id n):s(x),i(n){}Connection(Connection&&o)noexcept:s(o.s),i(o.i){o.s=nullptr;}~Connection(){Disconnect();}Connection(const Connection&)=delete;void Disconnect(){if(s){s->Disconnect(i);s=nullptr;}}explicit operator bool()const{return s!=nullptr;}};Connection Connect(Callback cb){std::lock_guard<std::mutex>l(m);auto i=next++;slots[i]=std::move(cb);return Connection(this,i);}void Disconnect(Id i){std::lock_guard<std::mutex>l(m);slots.erase(i);}void Clear(){std::lock_guard<std::mutex>l(m);slots.clear();}void Fire(A...a){std::vector<Callback>v;{std::lock_guard<std::mutex>l(m);for(auto&[_,cb]:slots)v.push_back(cb);}for(auto&cb:v)if(cb)cb(a...);}private:std::mutex m;std::unordered_map<Id,Callback>slots;Id next=1;};}
#pragma once
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
namespace rsm {
class DeferredDestruction {
 mutable std::mutex m_;std::queue<std::function<void()>>q_;
public:
 void Defer(std::function<void()>fn){std::lock_guard<std::mutex>l(m_);q_.push(std::move(fn));}
 size_t Drain(size_t budget=128){size_t n=0;while(n<budget){std::function<void()>fn;{std::lock_guard<std::mutex>l(m_);if(q_.empty())break;fn=std::move(q_.front());q_.pop();}if(fn)fn();++n;}return n;}
 size_t Pending()const{std::lock_guard<std::mutex>l(m_);return q_.size();}
};
class ResourceCache {
 size_t budget_=256ull*1024*1024,used_=0;
public:
 void SetBudget(size_t b){budget_=b;}bool Reserve(size_t n){if(n>budget_-used_)return false;used_+=n;return true;}void Release(size_t n){used_=n>used_?0:used_-n;}size_t Used()const{return used_;}size_t Budget()const{return budget_;}
}; }
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
namespace rsm {
class ActorScheduler {
 std::mutex m_;std::condition_variable cv_;std::queue<std::function<void()>> q_;std::vector<std::thread> workers_;std::atomic<bool>stop_{false};
 void Worker(){for(;;){std::function<void()> fn;{std::unique_lock<std::mutex>l(m_);cv_.wait(l,[&]{return stop_||!q_.empty();});if(stop_&&q_.empty())return;fn=std::move(q_.front());q_.pop();}try{if(fn)fn();}catch(...){}}}
public:
 explicit ActorScheduler(size_t n=1){for(size_t i=0;i<(n?n:1);++i)workers_.emplace_back([this]{Worker();});}
 ~ActorScheduler(){Shutdown();}
 void Enqueue(std::function<void()>fn){{std::lock_guard<std::mutex>l(m_);if(stop_)return;q_.push(std::move(fn));}cv_.notify_one();}
 void Shutdown(){bool expected=false;if(!stop_.compare_exchange_strong(expected,true))return;cv_.notify_all();for(auto&t:workers_)if(t.joinable())t.join();}
 size_t Pending()const{std::lock_guard<std::mutex>l(m_);return q_.size();}
};
}
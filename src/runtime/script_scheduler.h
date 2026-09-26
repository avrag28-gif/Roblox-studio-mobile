#pragma once
#include <functional>
#include <queue>
#include <cstdint>
namespace rsm {
class ScriptScheduler{
 struct Job{uint64_t id;std::function<void()>fn;};std::queue<Job>q_;uint64_t next_=1;
public:
 uint64_t Enqueue(std::function<void()> fn){auto id=next_++;q_.push({id,std::move(fn)});return id;}
 size_t Step(size_t budget=32){size_t n=0;while(n<budget&&!q_.empty()){auto j=std::move(q_.front());q_.pop();if(j.fn)j.fn();++n;}return n;}
 size_t Pending()const{return q_.size();}
};}
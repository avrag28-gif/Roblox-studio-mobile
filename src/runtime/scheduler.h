#pragma once
#include <functional>
#include <queue>
namespace rsm {
class ScriptScheduler {
 std::queue<std::function<void()>> ready_;
public:
 void Enqueue(std::function<void()> fn){ready_.push(std::move(fn));}
 size_t Step(size_t budget=64){size_t n=0;while(n<budget&&!ready_.empty()){auto fn=std::move(ready_.front());ready_.pop();if(fn)fn();++n;}return n;}
 size_t Pending()const{return ready_.size();}
};
}
#pragma once
#include "../core/instance.h"
#include <cstdint>
#include <functional>
#include <queue>
namespace rsm {
struct ReplicationMessage{uint64_t sequence=0;std::string path;std::string property;std::string value;};
class ReplicationRuntime {
 uint64_t next_=1,lastApplied_=0;std::queue<ReplicationMessage>out_;
public:
 void Queue(std::string path,std::string property,std::string value){out_.push({next_++,std::move(path),std::move(property),std::move(value)});}
 bool Pop(ReplicationMessage&m){if(out_.empty())return false;m=std::move(out_.front());out_.pop();return true;}
 bool ApplyOrdered(const ReplicationMessage&m,const std::function<bool(const ReplicationMessage&)>&fn){if(m.sequence<=lastApplied_)return false;if(!fn(m))return false;lastApplied_=m.sequence;return true;}
 uint64_t LastApplied()const{return lastApplied_;}
};
}
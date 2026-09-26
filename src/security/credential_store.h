#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
namespace rsm {
class CredentialStore {
 std::unordered_map<std::string,std::string> values_; mutable std::mutex m_;
public:
 virtual ~CredentialStore()=default;
 virtual bool Put(const std::string&key,const std::string&value){std::lock_guard<std::mutex>l(m_);if(key.empty()||value.empty())return false;values_[key]=value;return true;}
 virtual std::string Get(const std::string&key)const{std::lock_guard<std::mutex>l(m_);auto i=values_.find(key);return i==values_.end()?"":i->second;}
 virtual void Erase(const std::string&key){std::lock_guard<std::mutex>l(m_);values_.erase(key);}
 virtual void Clear(){std::lock_guard<std::mutex>l(m_);values_.clear();}
};
}
#pragma once
#include <string>
#include <unordered_set>
#include <array>
namespace rsm {
class ScriptSandbox{
 public:
  void Allow(std::string capability){allowed_.insert(std::move(capability));}
  bool Can(const std::string& capability)const{return allowed_.count(capability)>0;}
  bool ValidateSource(const std::string&s,std::string&error)const{
    static constexpr std::array<const char*,8> blocked={"io.","os.","require("ffi")","debug.","loadstring","dofile","writefile","readfile"};
    for(auto*b:blocked)if(s.find(b)!=std::string::npos){error=std::string("blocked capability: ")+b;return false;}
    if(s.size()>maxSourceBytes_){error="script exceeds sandbox source limit";return false;}
    error.clear();return true;
  }
  void SetMaxSourceBytes(std::size_t n){maxSourceBytes_=n;}
  std::size_t MaxSourceBytes()const{return maxSourceBytes_;}
 private:
  std::unordered_set<std::string>allowed_;
  std::size_t maxSourceBytes_=512*1024;
};
}
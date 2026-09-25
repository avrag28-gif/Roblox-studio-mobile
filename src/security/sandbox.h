#pragma once
#include <string>
#include <unordered_set>
namespace rsm {
class ScriptSandbox{
 public:
  void Allow(std::string capability){allowed_.insert(std::move(capability));}
  bool Can(const std::string& capability)const{return allowed_.count(capability)>0;}
  bool ValidateSource(const std::string&s,std::string&error)const{
    static const char*blocked[]={"io.","os.","require(\"ffi\")","debug."};
    for(auto*b:blocked)if(s.find(b)!=std::string::npos){error=std::string("blocked capability: ")+b;return false;}
    error.clear();return true;
  }
 private:std::unordered_set<std::string>allowed_;
};
}
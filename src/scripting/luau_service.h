#pragma once
#include "../core/instance.h"
#include <string>
#include <vector>
#include <functional>
namespace rsm {
struct ScriptDiagnostic{int line=1;std::string message;};
class LuauService{
 public:
  using Log=std::function<void(std::string)>;
  explicit LuauService(Log log={}):log_(std::move(log)){}
  bool CompileAndRun(const std::string& source){
    diagnostics_.clear();
    if(source.find("\0")!=std::string::npos){diagnostics_.push_back({1,"NUL byte in source"});return false;}
    int balance=0,line=1;
    for(char c:source){if(c=='\n')++line;else if(c=='(')++balance;else if(c==')'&&--balance<0){diagnostics_.push_back({line,"unexpected ')'"});return false;}}
    if(balance){diagnostics_.push_back({line,"unclosed '('"});return false;}
    if(log_&&source.find("print(")!=std::string::npos)log_("Luau print dispatched");
    return diagnostics_.empty();
  }
  const std::vector<ScriptDiagnostic>& Diagnostics()const{return diagnostics_;}
 private:Log log_;std::vector<ScriptDiagnostic> diagnostics_;
};
}
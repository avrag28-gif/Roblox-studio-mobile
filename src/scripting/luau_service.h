#pragma once
#include <functional>
#include <string>
#include <vector>
#ifdef RSM_LUAU_ENABLED
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
}
#endif
namespace rsm {
struct ScriptDiagnostic{int line=1;std::string message;};
class LuauService{
 public:
  using Log=std::function<void(std::string)>;
  explicit LuauService(Log log={}):log_(std::move(log)){}
  bool CompileAndRun(const std::string& source){
    diagnostics_.clear();
#ifdef RSM_LUAU_ENABLED
    lua_State* L=luaL_newstate();
    if(!L){diagnostics_.push_back({1,"failed to create Luau VM"});return false;}
    luaL_sandbox(L); luaL_sandboxthread(L); luaL_openlibs(L);
    size_t bytecodeSize=0; char* bytecode=luau_compile(source.data(),source.size(),nullptr,&bytecodeSize);
    if(!bytecode){diagnostics_.push_back({1,"Luau compiler returned no bytecode"});lua_close(L);return false;}
    int load=luau_load(L,"=RSM",bytecode,bytecodeSize,0); free(bytecode);
    if(load!=0){diagnostics_.push_back({1,lua_tostring(L,-1)?lua_tostring(L,-1):"Luau load error"});lua_close(L);return false;}
    int run=lua_pcall(L,0,0,0);
    if(run!=0){diagnostics_.push_back({1,lua_tostring(L,-1)?lua_tostring(L,-1):"Luau runtime error"});lua_close(L);return false;}
    if(log_&&source.find("print(")!=std::string::npos)log_("Luau script executed");
    lua_close(L); return true;
#else
    int balance=0,line=1; for(char c:source){if(c=='\n')++line;else if(c=='(')++balance;else if(c==')'&&--balance<0){diagnostics_.push_back({line,"unexpected ')'"});return false;}}
    if(balance){diagnostics_.push_back({line,"unclosed '('"});return false;}
    if(log_&&source.find("print(")!=std::string::npos)log_("Luau-compatible script dispatched");
    return diagnostics_.empty();
#endif
  }
  const std::vector<ScriptDiagnostic>&Diagnostics()const{return diagnostics_;}
 private:Log log_;std::vector<ScriptDiagnostic>diagnostics_;
};
}
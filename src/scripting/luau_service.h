#pragma once
#include <functional>
#include <string>
#include <vector>
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../security/sandbox.h"
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
  void Bind(DataModel* dm){game_=dm;}
  bool CompileAndRun(const std::string& source){
    diagnostics_.clear();std::string sandboxError;if(!sandbox_.ValidateSource(source,sandboxError)){diagnostics_.push_back({1,sandboxError});return false;}
#ifdef RSM_LUAU_ENABLED
    lua_State* L=luaL_newstate();if(!L){diagnostics_.push_back({1,"failed to create Luau VM"});return false;}
    luaL_sandbox(L);luaL_sandboxthread(L);luaL_openlibs(L);
    lua_pushlightuserdata(L,game_);lua_setglobal(L,"__rsm_game");
    lua_newtable(L);lua_pushcfunction(L,&GetServiceThunk);lua_setfield(L,-2,"GetService");lua_setglobal(L,"game");
    lua_newtable(L);lua_pushcfunction(L,&InstanceNewThunk);lua_setfield(L,-2,"new");lua_setglobal(L,"Instance");
    size_t bytecodeSize=0;char* bytecode=luau_compile(source.data(),source.size(),nullptr,&bytecodeSize);
    if(!bytecode){diagnostics_.push_back({1,"Luau compiler returned no bytecode"});lua_close(L);return false;}
    int load=luau_load(L,"=RSM",bytecode,bytecodeSize,0);free(bytecode);
    if(load!=0){diagnostics_.push_back({1,ErrorText(L)});lua_close(L);return false;}
    int run=lua_pcall(L,0,0,0);if(run!=0){diagnostics_.push_back({1,ErrorText(L)});lua_close(L);return false;}
    if(log_&&source.find("print(")!=std::string::npos)log_("Luau script executed");lua_close(L);return true;
#else
    int balance=0,line=1;for(char c:source){if(c=='\n')++line;else if(c=='(')++balance;else if(c==')'&&--balance<0){diagnostics_.push_back({line,"unexpected ')'"});return false;}}if(balance){diagnostics_.push_back({line,"unclosed '('"});return false;}if(log_&&source.find("print(")!=std::string::npos)log_("Luau-compatible script dispatched");return true;
#endif
  }
  const std::vector<ScriptDiagnostic>&Diagnostics()const{return diagnostics_;}
  ScriptSandbox&Sandbox(){return sandbox_;}
 private:
#ifdef RSM_LUAU_ENABLED
  static std::string ErrorText(lua_State*L){const char*s=lua_tostring(L,-1);return s?s:"Luau error";}
  static int GetServiceThunk(lua_State*L){auto*dm=static_cast<DataModel*>(lua_touserdata(L,lua_upvalueindex(1)));if(!dm){lua_pushnil(L);return 1;}const char*n=luaL_checkstring(L,1);auto*i=dm->GetService(n);if(!i){lua_pushnil(L);return 1;}lua_pushlightuserdata(L,i);return 1;}
  static int InstanceNewThunk(lua_State*L){const char*n=luaL_checkstring(L,1);auto p=InstanceFactory::New(n);if(!p){lua_pushnil(L);return 1;}auto*i=p.get();auto*dm=static_cast<DataModel*>(lua_touserdata(L,lua_upvalueindex(1)));if(dm)Instance::SetParent(std::move(p),dm);lua_pushlightuserdata(L,i);return 1;}
#endif
  Log log_;std::vector<ScriptDiagnostic>diagnostics_;ScriptSandbox sandbox_;DataModel*game_=nullptr;
};
}
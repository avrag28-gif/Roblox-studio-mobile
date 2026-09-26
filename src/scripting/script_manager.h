#pragma once
#include "luau_service.h"
#include "../core/class_system.h"
#include <unordered_set>
namespace rsm {
enum class ScriptKind { Server, Client, Module };
enum class ScriptState { Stopped, Running, Failed };
class ScriptManager {
 DataModel* game_=nullptr; LuauService* luau_=nullptr;
 std::unordered_map<Script*,ScriptState> states_;
 std::unordered_set<ModuleScript*> modules_;
public:
 void Bind(DataModel*game,LuauService*service){game_=game;luau_=service;states_.clear();modules_.clear();}
 bool StartScript(Script*script,ScriptKind kind){
  if(!script||!luau_)return false;
  states_[script]=ScriptState::Running;
  bool ok=luau_->CompileAndRun(script->Source());
  states_[script]=ok?ScriptState::Running:ScriptState::Failed;
  return ok;
 }
 void StopScript(Script*script){if(script)states_[script]=ScriptState::Stopped;}
 bool RunServerScripts(){
  if(!game_)return false;auto*s=game_->GetService("ServerScriptService");if(!s)return false;bool ok=true;
  for(auto*i:s->GetDescendants())if(auto*x=dynamic_cast<Script*>(i))if(!dynamic_cast<LocalScript*>(x)&&!dynamic_cast<ModuleScript*>(x))ok=StartScript(x,ScriptKind::Server)&&ok;return ok;
 }
 bool RunClientScripts(){
  if(!game_)return false;bool ok=true;
  for(auto*i:game_->GetDescendants())if(auto*x=dynamic_cast<LocalScript*>(i))ok=StartScript(x,ScriptKind::Client)&&ok;return ok;
 }
 bool Require(ModuleScript*module){
  if(!module||!luau_)return false;modules_.insert(module);return StartScript(module,ScriptKind::Module);
 }
 ScriptState State(Script*script)const{auto i=states_.find(script);return i==states_.end()?ScriptState::Stopped:i->second;}
};
}
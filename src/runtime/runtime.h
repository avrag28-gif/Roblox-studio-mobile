#pragma once
#include "../core/data_model.h"
#include "../physics/physics_world.h"
#include "../scripting/luau_service.h"
namespace rsm {
class Runtime {
 public:
  Runtime():script_([this](std::string m){lastLog_=std::move(m);}) {}
  bool Start(const DataModel& source){
    auto copy=source.Clone();
    if(!copy)return false;
    auto* dm=dynamic_cast<DataModel*>(copy.release());
    if(!dm)return false;
    runtime_=std::unique_ptr<DataModel>(dm);
    physics_.Clear();
    running_=true;
    return true;
  }
  bool RunScript(const std::string& source){
    if(!running_)return false;
    return script_.CompileAndRun(source);
  }
  void Step(float dt){if(running_&&runtime_)physics_.Step(dt,*runtime_);}
  void Stop(){running_=false;runtime_.reset();physics_.Clear();}
  bool Running()const{return running_&&runtime_!=nullptr;}
  DataModel* Game(){return runtime_.get();}
  const DataModel* Game()const{return runtime_.get();}
  const std::string& LastLog()const{return lastLog_;}
  const LuauService& Scripts()const{return script_;}
 private:
  std::unique_ptr<DataModel> runtime_;
  PhysicsWorld physics_;
  LuauService script_;
  std::string lastLog_;
  bool running_=false;
};
}
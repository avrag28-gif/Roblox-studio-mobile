#pragma once
#include "../scene/scene.h"
#include "../physics/physics_world.h"
#include "../scripting/luau_service.h"
namespace rsm {
class Runtime {
 public:
  Runtime():script_([this](std::string m){lastLog_=std::move(m);}) {}
  bool Start(const Scene& source){
    auto copy=source.Game().Clone();
    if(!copy)return false;
    scene_.Reset();
    auto* dm=dynamic_cast<DataModel*>(copy.release());
    if(!dm)return false;
    runtimeRoot_.reset(dm);
    running_=true;
    return true;
  }
  bool RunScript(const std::string& source){
    if(!running_)return false;
    return script_.CompileAndRun(source);
  }
  void Step(float dt){if(running_)physics_.Step(dt,scene_.Game());}
  void Stop(){running_=false;runtimeRoot_.reset();}
  bool Running()const{return running_;}
  Scene& Game(){return scene_;}
  const Scene& Game()const{return scene_;}
  const std::string& LastLog()const{return lastLog_;}
  const LuauService& Scripts()const{return script_;}
 private:
  Scene scene_;
  PhysicsWorld physics_;
  LuauService script_;
  std::unique_ptr<DataModel> runtimeRoot_;
  std::string lastLog_;
  bool running_=false;
};
}
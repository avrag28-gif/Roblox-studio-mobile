#pragma once
#include "../core/data_model.h"
#include "../core/base_part.h"
#include <memory>
namespace rsm {
class SceneRuntime {
 public:
  explicit SceneRuntime(DataModel& source):source_(source){}
  bool Start(){ if(active_) return false; snapshot_=source_.Clone(); active_=snapshot_!=nullptr; return active_; }
  void Stop(){snapshot_.reset();active_=false;}
  bool Active()const{return active_;}
  Instance* Root()const{return snapshot_.get();}
 private: DataModel& source_; std::unique_ptr<Instance> snapshot_; bool active_=false;
};
}
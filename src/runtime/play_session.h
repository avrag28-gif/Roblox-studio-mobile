#pragma once
#include "../scene/scene.h"
namespace rsm {
class PlaySession {
 Scene runtime_;bool running_=false;
public:
 bool Start(const DataModel&source){
  auto clone=source.Clone();
  auto* data=clone?dynamic_cast<DataModel*>(clone.get()):nullptr;
  if(!data)return false;
  runtime_.Reset();
  runtime_.Game().ReplaceContentsFrom(*data);
  running_=true;
  return true;
 }
 void Stop(){running_=false;runtime_.Reset();}
 bool Start(const Scene&source){return Start(source.Game());}
 bool Running()const{return running_;}
 Scene&SceneData(){return runtime_;}
};
}

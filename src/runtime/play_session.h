#pragma once
#include "../scene/scene.h"
namespace rsm {
class PlaySession {
 Scene runtime_;bool running_=false;
public:
 bool Start(const Scene&source){auto clone=source.Game().Clone();if(!clone)return false;runtime_.Reset();runtime_.Game().SetName(clone->Name());for(auto*x:clone->GetChildren()){auto c=x->Clone();if(c)Instance::SetParent(std::move(c),&runtime_.Game());}running_=true;return true;}
 void Stop(){running_=false;runtime_.Reset();}
 bool Running()const{return running_;}Scene&SceneData(){return runtime_;}
};
}
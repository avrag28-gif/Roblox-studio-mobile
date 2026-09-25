#pragma once
#include "../scene/scene.h"
namespace rsm { class PlaySession{Scene runtime_;bool running_=false;public:bool Start(const Scene& source){runtime_.Reset();(void)source;running_=true;return true;}void Stop(){running_=false;}bool Running()const{return running_;}Scene&SceneData(){return runtime_;}};}
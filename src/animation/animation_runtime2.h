#pragma once
#include "../math/vector3.h"
#include <algorithm>
#include <string>
#include <vector>
namespace rsm {
struct Keyframe{float time=0;Vector3 position{};};
class AnimationTrack {std::vector<Keyframe> keys_;float length_=0;bool playing_=false;float t_=0;public:
 void Add(Keyframe k){keys_.push_back(k);std::sort(keys_.begin(),keys_.end(),[](auto&a,auto&b){return a.time<b.time;});length_=keys_.empty()?0:keys_.back().time;}
 void Play(){playing_=true;t_=0;} void Stop(){playing_=false;t_=0;} void Step(float dt){if(playing_&&length_>0){t_+=dt;if(t_>length_)t_=0;}}
 Vector3 Sample()const{if(keys_.empty())return{};if(keys_.size()==1)return keys_[0].position;for(size_t i=1;i<keys_.size();++i)if(t_<=keys_[i].time){auto&a=keys_[i-1];auto&b=keys_[i];float u=(t_-a.time)/(b.time-a.time);return a.position*(1-u)+b.position*u;}return keys_.back().position;}
};
}
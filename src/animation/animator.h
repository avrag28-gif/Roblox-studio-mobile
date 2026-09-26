#pragma once
#include "../math/cframe.h"
#include <algorithm>
#include <vector>
namespace rsm {
struct Keyframe { float time=0; CFrame transform{}; };
class AnimationTrack {
 std::vector<Keyframe> keys_; float length_=0,time_=0; bool playing_=false,looped_=false;
public:
 void AddKeyframe(Keyframe k){keys_.push_back(k);length_=std::max(length_,k.time);}
 void Play(bool looped=false){looped_=looped;playing_=true;}
 void Stop(){playing_=false;time_=0;}
 void Step(float dt){if(!playing_||length_<=0)return;time_+=dt;if(time_>length_){if(looped_)time_=0;else{time_=length_;playing_=false;}}}
 float Time()const{return time_;} bool Playing()const{return playing_;}
 CFrame Sample()const{if(keys_.empty())return {};if(time_<=keys_.front().time)return keys_.front().transform;for(size_t i=1;i<keys_.size();++i)if(time_<=keys_[i].time){float d=keys_[i].time-keys_[i-1].time;float a=d>0?(time_-keys_[i-1].time)/d:0;return CFrame(keys_[i-1].transform.position*(1-a)+keys_[i].transform.position*a);}return keys_.back().transform;}
};
}
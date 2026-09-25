#pragma once
#include "../math/math3d.h"
#include <string>
#include <vector>
namespace rsm {
struct Keyframe{float time=0;Vector3 position{};Vector3 rotation{};Vector3 scale{1,1,1};};
struct AnimationTrack{std::string name;std::vector<Keyframe>keys;};
class Animator{
 public:
  void SetTrack(AnimationTrack t){track_=std::move(t);time_=0;} void Step(float dt){time_+=dt;}
  Keyframe Sample()const{if(track_.keys.empty())return{};if(time_<=track_.keys.front().time)return track_.keys.front();if(time_>=track_.keys.back().time)return track_.keys.back();for(size_t i=1;i<track_.keys.size();++i)if(time_<=track_.keys[i].time){auto&a=track_.keys[i-1];auto&b=track_.keys[i];float t=(time_-a.time)/(b.time-a.time);return{time_,a.position*(1-t)+b.position*t,a.rotation*(1-t)+b.rotation*t,a.scale*(1-t)+b.scale*t};}return track_.keys.back();}
 private:AnimationTrack track_;float time_=0;
};
}
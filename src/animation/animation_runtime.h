#pragma once
#include "../core/base_part.h"
#include "../math/math3d.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
namespace rsm {
struct Keyframe{float time=0;Vector3 position{};Vector3 rotation{};Vector3 scale{1,1,1};};
struct AnimationTrack{std::string name;std::vector<Keyframe>keys;float length=0;};
class Animator{
 AnimationTrack track_;float time_=0;bool playing_=false,loop_=false;
public:
 void SetTrack(AnimationTrack t){std::sort(t.keys.begin(),t.keys.end(),[](auto&a,auto&b){return a.time<b.time;});track_=std::move(t);time_=0;}
 void Play(bool loop=false){loop_=loop;playing_=true;}
 void Pause(){playing_=false;}
 void Stop(){playing_=false;time_=0;}
 bool Playing()const{return playing_;}
 void SetTime(float t){time_=std::max(0.0f,t);}
 float Time()const{return time_;}
 void Step(float dt){if(!playing_||track_.keys.empty())return;time_+=std::max(0.0f,dt);float end=track_.length>0?track_.length:track_.keys.back().time;if(end>0&&time_>=end){if(loop_)time_=std::fmod(time_,end);else{time_=end;playing_=false;}}}
 Keyframe Sample()const{
  if(track_.keys.empty())return{}; if(time_<=track_.keys.front().time)return track_.keys.front();
  if(time_>=track_.keys.back().time)return track_.keys.back();
  for(size_t i=1;i<track_.keys.size();++i)if(time_<=track_.keys[i].time){
   auto&a=track_.keys[i-1];auto&b=track_.keys[i];float d=b.time-a.time;float t=d>0?(time_-a.time)/d:0;
   return{time_,a.position*(1-t)+b.position*t,a.rotation*(1-t)+b.rotation*t,a.scale*(1-t)+b.scale*t};
  } return track_.keys.back();
 }
 void Apply(BasePart&part)const{auto k=Sample();auto cf=part.CFrameValue();cf.position=k.position;cf.rotation=k.rotation;part.SetCFrame(cf);part.SetSize(k.scale);}
};
}
#pragma once
#include "../math/vector3.h"
#include <cmath>
#include <string>
#include <unordered_map>
namespace rsm {
struct SoundState { std::string id; Vector3 position{}; float volume=1; bool looping=false,playing=false; };
class AudioWorld {
 std::unordered_map<std::string,SoundState> sounds_; uint64_t next_=1;
public:
 std::string Create(Vector3 position){std::string id="sound-"+std::to_string(next_++);Register({id,position});return id;}
 bool Register(SoundState s){return sounds_.emplace(s.id,std::move(s)).second;}
 bool Play(const std::string&id){auto i=sounds_.find(id);if(i==sounds_.end())return false;i->second.playing=true;return true;}
 bool Stop(const std::string&id){auto i=sounds_.find(id);if(i==sounds_.end())return false;i->second.playing=false;return true;}
 void SetVolume(const std::string&id,float v){auto i=sounds_.find(id);if(i!=sounds_.end())i->second.volume=std::max(0.f,std::min(1.f,v));}
 float Gain(const std::string&id,Vector3 listener)const{auto i=sounds_.find(id);if(i==sounds_.end()||!i->second.playing)return 0;Vector3 d=i->second.position-listener;float dist=d.Length();return i->second.volume/(1.f+dist*dist);}
 const SoundState*Get(const std::string&id)const{auto i=sounds_.find(id);return i==sounds_.end()?nullptr:&i->second;}
};
}
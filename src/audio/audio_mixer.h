#pragma once
#include <string>
#include <unordered_map>
namespace rsm {
struct SoundState{float volume=1;bool looping=false,playing=false;};
class AudioMixer{
 public:
  void Play(const std::string&id){sounds_[id].playing=true;} void Pause(const std::string&id){sounds_[id].playing=false;}
  void Stop(const std::string&id){sounds_.erase(id);} void SetVolume(const std::string&id,float v){sounds_[id].volume=v<0?0:v>1?1:v;}
  const SoundState* Get(const std::string&id)const{auto i=sounds_.find(id);return i==sounds_.end()?nullptr:&i->second;}
 private:std::unordered_map<std::string,SoundState>sounds_;
};
}
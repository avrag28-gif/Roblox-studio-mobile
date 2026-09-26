#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
namespace rsm {
struct MixerVoice {float volume=1,pan=0;bool looping=false,playing=false;};
class AudioMixer {
 std::unordered_map<std::string,MixerVoice> named_;std::vector<MixerVoice> voices_;float master_=1;
public:
 explicit AudioMixer(size_t maxVoices=32):voices_(maxVoices){}
 MixerVoice* Get(const std::string&id){return &named_[id];}
 const MixerVoice* Get(const std::string&id)const{auto i=named_.find(id);return i==named_.end()?nullptr:&i->second;}
 void Play(const std::string&id){named_[id].playing=true;}
 void Pause(const std::string&id){auto v=Get(id);v->playing=false;}
 void SetVolume(const std::string&id,float v){Get(id)->volume=std::clamp(v,0.f,1.f);}
 void SetLooping(const std::string&id,bool v){Get(id)->looping=v;}
 void SetMaster(float v){master_=std::clamp(v,0.f,1.f);}
 size_t AddVoice(){for(size_t i=0;i<voices_.size();++i)if(!voices_[i].playing){voices_[i].playing=true;return i;}return voices_.size();}
 void Stop(size_t i){if(i<voices_.size())voices_[i].playing=false;}
 void SetVoice(size_t i,float volume,float pan=0){if(i<voices_.size()){voices_[i].volume=std::clamp(volume,0.f,1.f);voices_[i].pan=std::clamp(pan,-1.f,1.f);}}
 void Mix(float*dst,size_t frames,size_t channels)const{
  if(!dst||channels==0)return;std::fill(dst,dst+frames*channels,0.f);
  for(const auto&v:voices_)if(v.playing){float g=v.volume*master_;for(size_t f=0;f<frames;++f){if(channels==1)dst[f]+=g;else{dst[f*channels]+=g*(1-v.pan)*.5f;dst[f*channels+1]+=g*(1+v.pan)*.5f;}}}
  for(const auto&kv:named_)if(kv.second.playing){float g=kv.second.volume*master_;for(size_t f=0;f<frames;++f)for(size_t ch=0;ch<channels;++ch)dst[f*channels+ch]+=g;}
 }
};
}
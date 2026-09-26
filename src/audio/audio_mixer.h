#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>
namespace rsm {
struct MixerVoice {float volume=1,pan=0;bool playing=false;};
class AudioMixer {
 std::vector<MixerVoice> voices_;float master_=1;
public:
 explicit AudioMixer(size_t maxVoices=32):voices_(maxVoices){}
 void SetMaster(float v){master_=std::clamp(v,0.f,1.f);}
 size_t AddVoice(){for(size_t i=0;i<voices_.size();++i)if(!voices_[i].playing){voices_[i].playing=true;return i;}return voices_.size();}
 void Stop(size_t i){if(i<voices_.size())voices_[i].playing=false;}
 void SetVoice(size_t i,float volume,float pan=0){if(i<voices_.size()){voices_[i].volume=std::clamp(volume,0.f,1.f);voices_[i].pan=std::clamp(pan,-1.f,1.f);}}
 void Mix(float*dst,size_t frames,size_t channels)const{
  if(!dst||channels==0)return;std::fill(dst,dst+frames*channels,0.f);
  for(const auto&v:voices_)if(v.playing){float g=v.volume*master_;for(size_t f=0;f<frames;++f){if(channels==1)dst[f]+=g;else{dst[f*channels]+=g*(1-v.pan)*.5f;dst[f*channels+1]+=g*(1+v.pan)*.5f;}}}
 }
};
}
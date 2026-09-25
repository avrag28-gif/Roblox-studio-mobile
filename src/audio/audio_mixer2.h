#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>
namespace rsm {
class AudioMixer {
 float master_=1;
public:
 void SetMaster(float v){master_=std::clamp(v,0.f,1.f);}
 float Master()const{return master_;}
 void Mix(const float*in,float*out,size_t frames,size_t channels,float gain=1)const{float g=master_*std::max(0.f,gain);for(size_t i=0;i<frames*channels;++i)out[i]=in[i]*g;}
};
}
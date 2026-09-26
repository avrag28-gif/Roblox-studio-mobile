#pragma once
#include <cstdint>
#include <functional>
namespace rsm {
class AudioDevice {
public:
 using FillCallback=std::function<void(float*,uint32_t,uint32_t)>;
 virtual ~AudioDevice()=default;
 virtual bool Start(uint32_t sampleRate,uint32_t channels,FillCallback fill)=0;
 virtual void Stop()=0;
 virtual bool IsRunning() const=0;
};
class NullAudioDevice final: public AudioDevice {
 bool running_=false;
public:
 bool Start(uint32_t,uint32_t,FillCallback) override {running_=true;return true;}
 void Stop() override {running_=false;}
 bool IsRunning() const override{return running_;}
};
}
#pragma once
#include <string>
namespace rsm {struct SoundState{std::string id;float volume=1;bool looping=false;bool playing=false;};class AudioWorld{public:void Play(SoundState& s){s.playing=true;}void Stop(SoundState& s){s.playing=false;}};}

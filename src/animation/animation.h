#pragma once
#include "../math/cframe.h"
#include <vector>
namespace rsm {struct Keyframe{float time=0;CFrame transform{};};class AnimationTrack{public:void Add(Keyframe k){keys_.push_back(k);}const std::vector<Keyframe>& Keys()const{return keys_;}private:std::vector<Keyframe> keys_;};}

#pragma once
#include "../math/vector3.h"
#include <vector>
namespace rsm { struct Keyframe{float time=0;Vector3 position{};}; class AnimationTrack{std::vector<Keyframe> keys_;public:void Add(Keyframe k){keys_.push_back(k);}const std::vector<Keyframe>& Keys()const{return keys_;}};}
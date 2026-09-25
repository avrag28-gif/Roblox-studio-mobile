#pragma once
#include "../math/vector3.h"
#include <string>
#include <vector>
namespace rsm {struct Keyframe{float time=0;Vector3 position{};};struct AnimationTrack{std::string name;std::vector<Keyframe>keys;};class Animator{std::vector<AnimationTrack>tracks;float time=0;public:void Add(AnimationTrack t){tracks.push_back(std::move(t));}void Step(float dt){time+=dt;}float Time()const{return time;}};}
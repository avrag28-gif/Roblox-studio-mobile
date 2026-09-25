#pragma once
#include <cstdint>
namespace rsm{enum class Capability:std::uint32_t{None=0,SceneRead=1,SceneWrite=2,FileRead=4,FileWrite=8,Network=16};inline bool Has(Capability s,Capability b){return(static_cast<std::uint32_t>(s)&static_cast<std::uint32_t>(b))!=0;}}
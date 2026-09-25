#pragma once
#include <cstdint>
namespace rsm {enum class Capability:uint32_t{None=0,SceneRead=1,SceneWrite=2,ScriptRun=4,Network=8,Assets=16,Publish=32};class CapabilitySet{uint32_t bits=0;public:void Grant(Capability c){bits|=uint32_t(c);}bool Has(Capability c)const{return(bits&uint32_t(c))==uint32_t(c);}};}
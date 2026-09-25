#pragma once
#include <cstdint>
namespace rsm {enum class Capability:std::uint32_t{None=0,Scene=1,Assets=2,Network=4,FileIO=8};class CapabilitySet{public:explicit CapabilitySet(std::uint32_t bits=0):bits_(bits){}bool Allows(Capability c)const{return(bits_&static_cast<std::uint32_t>(c))!=0;}private:std::uint32_t bits_;};}

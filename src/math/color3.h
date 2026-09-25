#pragma once
namespace rsm { struct Color3 { float r=1,g=1,b=1; constexpr Color3()=default; constexpr Color3(float R,float G,float B):r(R),g(G),b(B){} }; }
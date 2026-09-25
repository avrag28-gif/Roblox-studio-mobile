#pragma once
#include <cstddef>
#include <chrono>
namespace rsm {
struct PerformanceBudget{size_t textureBytes=200u*1024*1024,physicsBytes=100u*1024*1024,scriptBytes=80u*1024*1024,guiBytes=80u*1024*1024;};
class FrameProfiler{std::chrono::steady_clock::time_point start_;double ms_=0;public:void Begin(){start_=std::chrono::steady_clock::now();}void End(){ms_=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start_).count();}double FrameMs()const{return ms_;}};
}
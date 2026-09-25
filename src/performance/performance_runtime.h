#pragma once
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <string>
namespace rsm {
struct FrameMetrics {double cpuMs=0,gpuMs=0,physicsMs=0,scriptMs=0;uint64_t drawCalls=0,triangles=0;size_t memoryBytes=0;};
class PerformanceRuntime {
 std::chrono::steady_clock::time_point start_; std::unordered_map<std::string,double> timers_; FrameMetrics frame_;
public:
 void BeginFrame(){start_=std::chrono::steady_clock::now();frame_={};}
 void EndFrame(){frame_.cpuMs=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start_).count();}
 void Set(const std::string&n,double ms){timers_[n]=ms;if(n=="physics")frame_.physicsMs=ms;if(n=="script")frame_.scriptMs=ms;if(n=="gpu")frame_.gpuMs=ms;}
 void SetDrawCalls(uint64_t n){frame_.drawCalls=n;} void SetMemory(size_t n){frame_.memoryBytes=n;}
 const FrameMetrics&Frame()const{return frame_;}
 bool Within(double budgetMs=16.67)const{return frame_.cpuMs<=budgetMs;}
};
}
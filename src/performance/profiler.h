#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
namespace rsm {class Profiler{using C=std::chrono::steady_clock;std::unordered_map<std::string,double>ms;public:class Scope{Profiler&p;std::string n;C::time_point t;public:Scope(Profiler&x,std::string s):p(x),n(std::move(s)),t(C::now()){}~Scope(){p.ms[n]+=std::chrono::duration<double,std::milli>(C::now()-t).count();}};double Milliseconds(const std::string&n)const{auto i=ms.find(n);return i==ms.end()?0:i->second;}void Reset(){ms.clear();}};}
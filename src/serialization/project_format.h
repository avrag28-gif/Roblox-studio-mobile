#pragma once
#include "../core/data_model.h"
#include <fstream>
#include <string>
namespace rsm {struct ProjectFormat{static constexpr int Version=2;static bool Save(const DataModel&g,const std::string&p){std::ofstream f(p);if(!f)return false;f<<"RSM_PROJECT "<<Version<<"\n";Write(f,g,0);return true;}static void Write(std::ostream&f,const Instance&i,int d){for(int n=0;n<d;n++)f<<"  ";f<<i.ClassName()<<" "<<i.Name()<<"\n";for(auto*x:i.GetChildren())Write(f,*x,d+1);}static bool LoadHeader(const std::string&p,int&v){std::ifstream f(p);std::string t;return bool(f>>t>>v)&&t=="RSM_PROJECT";}};}
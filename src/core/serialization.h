#pragma once
#include "instance.h"
#include <sstream>
#include <string>
namespace rsm {
class Serializer {
public:
 static constexpr int CurrentVersion=3;
 static std::string Save(const Instance&root){std::ostringstream o;o<<"RSM_PROJECT\n3\n";write(o,root,0);return o.str();}
 static bool LoadHeader(const std::string&s,int&version){std::istringstream in(s);std::string magic,line;if(!std::getline(in,magic)||magic!="RSM_PROJECT"||!std::getline(in,line))return false;try{version=std::stoi(line);}catch(...){return false;}return version>=1&&version<=CurrentVersion;}
private:
 static void write(std::ostringstream&o,const Instance&x,int d){o<<d<<"|"<<x.ClassName()<<"|"<<x.Name()<<"\n";for(auto*c:x.GetChildren())write(o,*c,d+1);}
};
}
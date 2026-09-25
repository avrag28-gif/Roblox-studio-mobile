#pragma once
#include "../core/instance.h"
#include <string>
#include <sstream>
namespace rsm {
class ProjectStore{
 public:
  static constexpr int CurrentVersion=3;
  static std::string Serialize(const Instance&root){std::ostringstream o;o<<"RSM3\n";Write(root,o,0);return o.str();}
  static bool Valid(const std::string&s){return s.rfind("RSM",0)==0&&s.find('\n')!=std::string::npos;}
 private:
  static void Write(const Instance&x,std::ostringstream&o,int d){o<<d<<"|"<<x.ClassName()<<"|"<<x.Name()<<"\n";for(auto*c:x.GetChildren())Write(*c,o,d+1);}
};
}
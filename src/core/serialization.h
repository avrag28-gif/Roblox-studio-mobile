#pragma once
#include "instance.h"
#include <sstream>
namespace rsm { class Serializer{public:static std::string Save(const Instance& root){std::ostringstream o;o<<"RSM1\n";write(o,root,0);return o.str();}private:static void write(std::ostringstream& o,const Instance& x,int d){o<<d<<" "<<x.ClassName()<<" "<<x.Name()<<"\n";for(auto* c:x.GetChildren())write(o,*c,d+1);}}; }

#pragma once
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../core/base_part.h"
#include "../core/class_system.h"
#include <sstream>
namespace rsm {
class SceneCodec {
 static std::string Hex(const std::string&s){static const char*h="0123456789ABCDEF";std::string o;for(unsigned char c:s){o+=h[c>>4];o+=h[c&15];}return o;}
 static std::string Unhex(const std::string&s){std::string o;for(size_t i=0;i+1<s.size();i+=2){auto n=[](char c){return c>='0'&&c<='9'?c-'0':c>='A'&&c<='F'?c-'A'+10:c-'A'+10;};o.push_back(char((n(s[i])<<4)|n(s[i+1])));}return o;}
 static void Write(const Instance&i,std::ostream&o,int depth){
  o<<depth<<'|'<<Hex(i.ClassName())<<'|'<<Hex(i.Name());
  if(auto*p=dynamic_cast<const BasePart*>(&i)){
   auto q=p->Position();auto z=p->Size();auto c=p->Color();
   o<<'|'<<q.x<<','<<q.y<<','<<q.z<<'|'<<z.x<<','<<z.y<<','<<z.z<<'|'<<c.r<<','<<c.g<<','<<c.b<<'|'<<p->Transparency()<<'|'<<p->Anchored()<<'|'<<p->CanCollide();
  }
  if(auto*s=dynamic_cast<const Script*>(&i))o<<'|'<<Hex(s->Source());
  o<<'\n';for(auto*x:i.GetChildren())Write(*x,o,depth+1);
 }
public:
 static std::string Save(const DataModel&g){std::ostringstream o;o<<"RSM_SCENE 1\n";Write(g,o,0);return o.str();}
 static bool Valid(const std::string&s){return s.rfind("RSM_SCENE 1\n",0)==0;}
};
}
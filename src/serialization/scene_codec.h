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
 static std::unique_ptr<DataModel> Load(const std::string&s,std::string&error){
  error.clear();if(!Valid(s)){error="invalid RSM scene header";return nullptr;}
  auto dm=std::make_unique<DataModel>();dm->InitializeDefaultServices();
  std::vector<Instance*> stack;std::istringstream in(s);std::string line;std::getline(in,line);
  while(std::getline(in,line)){
   if(line.empty())continue;std::vector<std::string> f;std::stringstream ss(line);std::string part;while(std::getline(ss,part,'|'))f.push_back(part);
   if(f.size()<3){error="malformed scene record";return nullptr;}
   int depth=0;try{depth=std::stoi(f[0]);}catch(...){error="invalid scene depth";return nullptr;}
   if(depth<0||depth>int(stack.size())){error="invalid scene hierarchy";return nullptr;}
   if(depth==0 && Unhex(f[1])=="DataModel"){stack.clear();continue;}
   if(depth==1 && Unhex(f[1])=="Service"){auto*svc=dm->GetService(Unhex(f[2]));if(!svc){error="unknown service";return nullptr;}stack.resize(1);stack[0]=svc;continue;}
   auto obj=InstanceFactory::New(Unhex(f[1]));if(!obj){error="unsupported instance";return nullptr;}obj->SetName(Unhex(f[2]));
   if(auto*p=dynamic_cast<BasePart*>(obj.get());p&&f.size()>=9){
    auto parse3=[](const std::string&v,Vector3&out){std::stringstream q(v);char c1,c2;if(!(q>>out.x>>c1>>out.y>>c2>>out.z)||c1!=','||c2!=',')return false;return true;};
    Vector3 pos,size; if(parse3(f[3],pos)&&parse3(f[4],size)){p->SetPosition(pos);p->SetSize(size);}
    std::stringstream col(f[5]);char c1,c2;Color3 color{};if(col>>color.r>>c1>>color.g>>c2>>color.b)p->SetColor(color);
    try{p->SetTransparency(std::stof(f[6]));p->SetAnchored(std::stoi(f[7])!=0);p->SetCanCollide(std::stoi(f[8])!=0);}catch(...){error="invalid part property";return nullptr;}
   }
   if(auto*script=dynamic_cast<Script*>(obj.get());script&&f.size()>=4)script->SetSource(Unhex(f[9]));
   Instance*parent=depth==0?dm.get():stack[depth-1];Instance::SetParent(std::move(obj),parent);
   if(depth<int(stack.size()))stack.resize(depth);stack.push_back(parent->GetChildren().back());
  }
  return dm;
 }
 static std::string Save(const DataModel&g){std::ostringstream o;o<<"RSM_SCENE 1\n";Write(g,o,0);return o.str();}
 static bool Valid(const std::string&s){return s.rfind("RSM_SCENE 1\n",0)==0;}
};
}
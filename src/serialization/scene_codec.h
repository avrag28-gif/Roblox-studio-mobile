#pragma once
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../core/base_part.h"
#include "../core/class_system.h"
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace rsm {
class SceneCodec {
 static std::string Hex(const std::string&s){static const char*h="0123456789ABCDEF";std::string o;for(unsigned char c:s){o+=h[c>>4];o+=h[c&15];}return o;}
 static std::string Unhex(const std::string&s){std::string o;for(size_t i=0;i+1<s.size();i+=2){auto n=[](char c){if(c>='0'&&c<='9')return c-'0';if(c>='A'&&c<='F')return c-'A'+10;if(c>='a'&&c<='f')return c-'a'+10;return 0;};o.push_back(char((n(s[i])<<4)|n(s[i+1])));}return o;}
 static std::string V3(float x,float y,float z){std::ostringstream o;o<<x<<','<<y<<','<<z;return o.str();}
 static bool Parse3(const std::string&v,Vector3&out){std::stringstream q(v);char a,b;if(!(q>>out.x>>a>>out.y>>b>>out.z)||a!=','||b!=',')return false;return true;}
 static bool Parse4(const std::string&v,float&a,float&b,float&c,float&d){std::stringstream q(v);char x,y,z;if(!(q>>a>>x>>b>>y>>c>>z>>d)||x!=','||y!=','||z!=',')return false;return true;}
 static void Write(const Instance&i,std::ostream&o,int depth){
  o<<depth<<'|'<<Hex(i.ClassName())<<'|'<<Hex(i.Name());
  if(auto*p=dynamic_cast<const BasePart*>(&i)){
   auto pos=p->Position(),size=p->Size(),col=p->Color(),q=p->CFrameValue().rotation;
   o<<'|'<<V3(pos.x,pos.y,pos.z)<<'|'<<V3(size.x,size.y,size.z)
    <<'|'<<V3(col.r,col.g,col.b)<<'|'<<p->Transparency()<<'|'<<p->Anchored()<<'|'<<p->CanCollide()
    <<'|'<<q.x<<','<<q.y<<','<<q.z<<','<<q.w;
  }
  if(auto*s=dynamic_cast<const Script*>(&i))o<<'|'<<Hex(s->Source());
  o<<'\n';
  for(auto*x:i.GetChildren())Write(*x,o,depth+1);
 }
public:
 static std::unique_ptr<DataModel> Load(const std::string&s,std::string&error){
  error.clear();if(!Valid(s)){error="invalid RSM scene header";return nullptr;}
  auto dm=std::make_unique<DataModel>();dm->InitializeDefaultServices();
  std::vector<Instance*>stack;std::istringstream in(s);std::string line;std::getline(in,line);
  while(std::getline(in,line)){
   if(line.empty())continue;
   std::vector<std::string>f;std::stringstream ss(line);std::string part;while(std::getline(ss,part,'|'))f.push_back(part);
   if(f.size()<3){error="malformed scene record";return nullptr;}
   int depth=0;try{depth=std::stoi(f[0]);}catch(...){error="invalid scene depth";return nullptr;}
   if(depth<0||depth>int(stack.size())){error="invalid scene hierarchy";return nullptr;}
   const std::string cls=Unhex(f[1]),name=Unhex(f[2]);
   if(depth==0&&cls=="DataModel"){stack.clear();continue;}
   if(depth==1&&cls=="Service"){
    auto*svc=dm->GetService(name);if(!svc){error="unknown service";return nullptr;}
    stack.resize(1);stack[0]=svc;continue;
   }
   auto obj=InstanceFactory::New(cls);if(!obj){error="unsupported instance: "+cls;return nullptr;}obj->SetName(name);
   if(auto*p=dynamic_cast<BasePart*>(obj.get())){
    if(f.size()>=10){
     Vector3 pos,size,colv;
     if(!Parse3(f[3],pos)||!Parse3(f[4],size)||!Parse3(f[5],colv)){error="invalid BasePart vector";return nullptr;}
     p->SetPosition(pos);p->SetSize(size);p->SetColor({colv.x,colv.y,colv.z});
     try{p->SetTransparency(std::stof(f[6]));p->SetAnchored(std::stoi(f[7])!=0);p->SetCanCollide(std::stoi(f[8])!=0);}catch(...){error="invalid BasePart property";return nullptr;}
     float qx,qy,qz,qw;if(f.size()>=10&&Parse4(f[9],qx,qy,qz,qw)){auto cf=p->CFrameValue();cf.rotation={qx,qy,qz,qw};p->SetCFrame(cf);}
    }
   }
   if(auto*script=dynamic_cast<Script*>(obj.get())){
    if(f.size()>=11)script->SetSource(Unhex(f[10]));
    else if(f.size()>=10&&cls=="Script")script->SetSource(Unhex(f[9]));
   }
   Instance*parent=depth==0?dm.get():stack[depth-1];
   if(!parent){error="missing scene parent";return nullptr;}
   Instance*raw=Instance::SetParent(std::move(obj),parent);if(!raw){error="failed to attach instance";return nullptr;}
   if(depth<int(stack.size()))stack.resize(depth);stack.push_back(raw);
  }
  return dm;
 }
 static std::string Save(const DataModel&g){std::ostringstream o;o<<"RSM_SCENE 2\n";Write(g,o,0);return o.str();}
 static bool Valid(const std::string&s){return s.rfind("RSM_SCENE 2\n",0)==0||s.rfind("RSM_SCENE 1\n",0)==0;}
};
}

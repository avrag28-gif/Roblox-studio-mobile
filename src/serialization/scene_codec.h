#pragma once
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../core/base_part.h"
#include "../core/class_system.h"
#include "../core/transform_hierarchy.h"
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
 static int ShapeCode(PartShape s){return static_cast<int>(s);} static int MaterialCode(Material m){return static_cast<int>(m);}
 static void Write(const Instance&i,std::ostream&o,int depth){
  o<<depth<<'|'<<Hex(i.ClassName())<<'|'<<Hex(i.Name())<<'|'<<Hex(i.Id())<<'|'<<i.Archivable();
  if(auto*m=dynamic_cast<const Model*>(&i)){\n   auto world=rsm::WorldCFrame(*m); auto pos=world.position,q=world.rotation;\n   o<<"|M|"<<V3(pos.x,pos.y,pos.z)<<'|'<<q.x<<','<<q.y<<','<<q.z<<','<<q.w;\n  }\n  if(auto*p=dynamic_cast<const BasePart*>(&i)){
   auto world=rsm::WorldCFrame(*p); auto pos=world.position,size=p->Size(),col=p->Color(),q=world.rotation;
   o<<'|'<<V3(pos.x,pos.y,pos.z)<<'|'<<V3(size.x,size.y,size.z)<<'|'<<V3(col.r,col.g,col.b)
    <<'|'<<p->Transparency()<<'|'<<p->Anchored()<<'|'<<p->CanCollide()<<'|'<<p->CanTouch()<<'|'<<p->CanQuery()<<'|'<<p->Mass()<<'|'<<ShapeCode(p->Shape())<<'|'<<MaterialCode(p->MaterialValue())
    <<'|'<<q.x<<','<<q.y<<','<<q.z<<','<<q.w;
  }
  if(auto*s=dynamic_cast<const Script*>(&i))o<<'|'<<Hex(s->Source());
  o<<'|'<<i.Attributes().size();
  for(const auto&a:i.Attributes()){
   o<<'|'<<Hex(a.first);
   if(std::holds_alternative<bool>(a.second))o<<"|b|"<<std::get<bool>(a.second);
   else if(std::holds_alternative<double>(a.second))o<<"|d|"<<std::get<double>(a.second);
   else if(std::holds_alternative<std::string>(a.second))o<<"|s|"<<Hex(std::get<std::string>(a.second));
   else o<<"|n|";
  }
  o<<'\n';for(auto*x:i.GetChildren())Write(*x,o,depth+1);
 }#pragma once
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
  error.clear();if(s.rfind("RSM_SCENE 2\n",0)!=0){error="unsupported scene version";return nullptr;}
  auto dm=std::make_unique<DataModel>();std::vector<Instance*>stack;std::istringstream in(s);std::string line;std::getline(in,line);
  while(std::getline(in,line)){
   if(line.empty())continue;
   std::vector<std::string>f;std::stringstream ss(line);std::string part;while(std::getline(ss,part,'|'))f.push_back(part);
   if(f.size()<6){error="malformed scene record";return nullptr;}
   int depth=0;try{depth=std::stoi(f[0]);}catch(...){error="invalid scene depth";return nullptr;}
   if(depth<0||depth>int(stack.size())){error="invalid scene hierarchy";return nullptr;}
   const std::string cls=Unhex(f[1]),name=Unhex(f[2]),id=Unhex(f[3]);
   if(depth==0&&cls=="DataModel"){dm->SetId(id);dm->SetArchivable(std::stoi(f[4])!=0);stack.clear();continue;}
   if(depth==1&&cls=="Service"){
    auto*svc=dm->GetService(name);if(!svc){error="unknown service";return nullptr;}svc->SetId(id);svc->SetArchivable(std::stoi(f[4])!=0);stack.resize(2);stack[1]=svc;continue;
   }
   auto obj=InstanceFactory::New(cls);if(!obj){error="unsupported instance: "+cls;return nullptr;}obj->SetName(name);obj->SetId(id);obj->SetArchivable(std::stoi(f[4])!=0);
   std::size_t attrIndex=5;
   if(auto*p=dynamic_cast<BasePart*>(obj.get())){
    if(f.size()<18){error="incomplete BasePart record";return nullptr;}
    Vector3 pos,size,colv;if(!Parse3(f[5],pos)||!Parse3(f[6],size)||!Parse3(f[7],colv)){error="invalid BasePart vector";return nullptr;}
    try{p->SetPosition(pos);p->SetSize(size);p->SetColor({colv.x,colv.y,colv.z});p->SetTransparency(std::stof(f[8]));p->SetAnchored(std::stoi(f[9])!=0);p->SetCanCollide(std::stoi(f[10])!=0);p->SetCanTouch(std::stoi(f[11])!=0);p->SetCanQuery(std::stoi(f[12])!=0);p->SetMass(std::stof(f[13]));p->SetShape(static_cast<PartShape>(std::stoi(f[14])));p->SetMaterial(static_cast<Material>(std::stoi(f[15])));float qx,qy,qz,qw;if(!Parse4(f[16],qx,qy,qz,qw)){error="invalid quaternion";return nullptr;}auto cf=p->CFrameValue();cf.rotation={qx,qy,qz,qw};p->SetCFrame(cf);}catch(...){error="invalid BasePart property";return nullptr;}attrIndex=17;
   }else if(auto*model=dynamic_cast<Model*>(obj.get())){\n    if(f.size()>=8&&f[5]=="M"){Vector3 pos;float qx,qy,qz,qw;if(!Parse3(f[6],pos)||!Parse4(f[7],qx,qy,qz,qw)){error="invalid Model pivot";return nullptr;}model->SetPivot(CFrame(pos,{qx,qy,qz,qw}));attrIndex=8;}\n   }else if(auto*script=dynamic_cast<Script*>(obj.get())){if(f.size()<6){error="missing script source";return nullptr;}script->SetSource(Unhex(f[5]));attrIndex=6;}
   if(attrIndex>=f.size()){error="missing attribute count";return nullptr;}
   std::size_t count=0;try{count=std::stoul(f[attrIndex]);}catch(...){error="invalid attribute count";return nullptr;}++attrIndex;
   for(std::size_t n=0;n<count;++n){
    if(attrIndex+2>=f.size()){error="truncated attribute";return nullptr;}
    std::string key=Unhex(f[attrIndex++]),type=f[attrIndex++],value=f[attrIndex++];
    if(type=="b")obj->SetAttribute(key,value=="1");else if(type=="d")try{obj->SetAttribute(key,std::stod(value));}catch(...){error="invalid attribute number";return nullptr;}else if(type=="s")obj->SetAttribute(key,Unhex(value));else if(type!="n"){error="invalid attribute type";return nullptr;}
   }
   Instance*parent=depth==0?dm.get():stack[depth-1];if(!parent){error="missing scene parent";return nullptr;}
   Instance*raw=Instance::SetParent(std::move(obj),parent);if(!raw){error="failed to attach instance";return nullptr;}
   if(depth<int(stack.size()))stack.resize(depth);stack.push_back(raw);
  }
  return dm;
 }
 static std::string Save(const DataModel&g){std::ostringstream o;o<<"RSM_SCENE 2\n";Write(g,o,0);return o.str();}
 static bool Valid(const std::string&s){return s.rfind("RSM_SCENE 2\n",0)==0;}
};
}

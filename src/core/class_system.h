#pragma once
#include "instance.h"
#include "../math/cframe.h"
namespace rsm {

class Service:public Instance{
public:
 explicit Service(std::string n):Instance(n){SetName(std::move(n));}
 std::unique_ptr<Instance>Clone()const override{
  auto c=std::make_unique<Service>(Name()); CopyStateTo(*c); CloneChildrenTo(*c); return c;
 }
};

class Workspace:public Service{
public:
 Workspace():Service("Workspace"){}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Workspace>();CopyStateTo(*c);CloneChildrenTo(*c);return c;}
};

#define RSM_SERVICE_CLASS(Name) \
class Name:public Service{public:Name():Service(#Name){}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Name>();CopyStateTo(*c);CloneChildrenTo(*c);return c;}};

RSM_SERVICE_CLASS(Players)
RSM_SERVICE_CLASS(Lighting)
RSM_SERVICE_CLASS(ReplicatedFirst)
RSM_SERVICE_CLASS(ReplicatedStorage)
RSM_SERVICE_CLASS(ServerScriptService)
RSM_SERVICE_CLASS(ServerStorage)
RSM_SERVICE_CLASS(StarterGui)
RSM_SERVICE_CLASS(StarterPack)
RSM_SERVICE_CLASS(StarterPlayer)
RSM_SERVICE_CLASS(SoundService)

#undef RSM_SERVICE_CLASS

class Folder:public Instance{
public:
 Folder():Instance("Folder"){}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Folder>();CopyStateTo(*c);CloneChildrenTo(*c);return c;}
};

class Model:public Instance{
public:
 Model():Instance("Model"){}
 const CFrame&Pivot()const{return pivot_;}
 void SetPivot(CFrame v){v.rotation=v.rotation.Normalized();pivot_=v;PropertyChanged.Fire("Pivot");PropertyChanged.Fire("CFrame");}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Model>();CopyStateTo(*c);c->pivot_=pivot_;CloneChildrenTo(*c);return c;}
private:CFrame pivot_{};
};

class Script:public Instance{
public:
 explicit Script(std::string type="Script"):Instance(std::move(type)){}
 const std::string&Source()const{return source_;}
 void SetSource(std::string s){source_=std::move(s);PropertyChanged.Fire("Source");}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Script>(ClassName());CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}
protected:std::string source_;
};
class LocalScript:public Script{
public:
 LocalScript():Script("LocalScript"){}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<LocalScript>();CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}
};
class ModuleScript:public Script{
public:
 ModuleScript():Script("ModuleScript"){}
 std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<ModuleScript>();CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}
};

}

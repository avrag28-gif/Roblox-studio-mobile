#pragma once
#include "instance.h"
namespace rsm{
class Service:public Instance{public:explicit Service(std::string n):Instance("Service"){SetName(std::move(n));}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Service>(Name());CopyStateTo(*c);CloneChildrenTo(*c);return c;}};
class Folder:public Instance{public:Folder():Instance("Folder"){}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Folder>();CopyStateTo(*c);CloneChildrenTo(*c);return c;}};
class Model:public Instance{public:Model():Instance("Model"){}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Model>();CopyStateTo(*c);CloneChildrenTo(*c);return c;}};
class Script:public Instance{public:explicit Script(std::string type="Script"):Instance(std::move(type)){}const std::string&Source()const{return source_;}void SetSource(std::string s){source_=std::move(s);PropertyChanged.Fire("Source");}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<Script>(ClassName());CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}protected:std::string source_;};
class LocalScript:public Script{public:LocalScript():Script("LocalScript"){}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<LocalScript>();CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}};
class ModuleScript:public Script{public:ModuleScript():Script("ModuleScript"){}std::unique_ptr<Instance>Clone()const override{auto c=std::make_unique<ModuleScript>();CopyStateTo(*c);c->source_=source_;CloneChildrenTo(*c);return c;}};
}
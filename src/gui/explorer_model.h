#pragma once
#include "../core/instance.h"
#include <vector>
namespace rsm {class ExplorerModel{public:std::vector<Instance*> Children(const Instance& root)const{return root.GetChildren();}std::vector<Instance*> Search(const Instance& root,const std::string& q)const{std::vector<Instance*> r;for(auto* x:root.GetDescendants())if(x->Name().find(q)!=std::string::npos)r.push_back(x);return r;}};}

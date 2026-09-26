#pragma once
#include "../core/instance.h"
#include <string>
namespace rsm { class ExplorerController{Instance*root_=nullptr;Instance*selected_=nullptr;public:void Bind(Instance*r){root_=r;selected_=nullptr;}Instance*Root()const{return root_;}void Select(Instance*i){selected_=i;}Instance*Selected()const{return selected_;}bool Rename(Instance*i,const std::string&n){if(!i||n.empty())return false;i->SetName(n);return true;}bool Reparent(Instance*i,Instance*p){if(!i||i==p)return false;i->SetParent(p);return i->Parent()==p;}};}
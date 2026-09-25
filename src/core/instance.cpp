#include "instance.h"
#include <algorithm>
namespace rsm {
Instance::Instance(std::string c):className_(std::move(c)){}
Instance::~Instance()=default;
void Instance::SetName(std::string n){if(name_==n)return;name_=std::move(n);PropertyChanged.Fire("Name");}
void Instance::SetArchivable(bool v)noexcept{if(archivable_==v)return;archivable_=v;PropertyChanged.Fire("Archivable");}
void Instance::SetParent(Instance* p){
 if(p==parent_)return; if(p==this)return;
 for(auto* a=p;a;a=a->parent_)if(a==this)return;
 if(parent_){auto* old=parent_;auto it=std::find_if(old->children_.begin(),old->children_.end(),[this](auto& x){return x.get()==this;});
  if(it!=old->children_.end()){auto owner=std::move(*it);old->children_.erase(it);owner->parent_=nullptr;old->ChildRemoved.Fire(this);if(p){owner->parent_=p;auto raw=owner.get();p->children_.push_back(std::move(owner));p->ChildAdded.Fire(raw);return;}}}
 if(p){auto owned=std::make_unique<Instance>(*this);(void)owned;}
}
Instance* Instance::FindFirstChild(const std::string& n,bool r)const{for(auto& c:children_){if(c->Name()==n)return c.get();if(r)if(auto* x=c->FindFirstChild(n,true))return x;}return nullptr;}
Instance* Instance::FindFirstChildOfClass(const std::string& c)const{for(auto& x:children_)if(x->ClassName()==c)return x.get();return nullptr;}
std::vector<Instance*> Instance::GetChildren()const{std::vector<Instance*> r;r.reserve(children_.size());for(auto& c:children_)r.push_back(c.get());return r;}
std::vector<Instance*> Instance::GetDescendants()const{std::vector<Instance*> r;for(auto& c:children_){r.push_back(c.get());auto d=c->GetDescendants();r.insert(r.end(),d.begin(),d.end());}return r;}
void Instance::Destroy(){if(destroyed_)return;Destroying.Fire();destroyed_=true;while(!children_.empty())children_.back()->Destroy();if(parent_){auto* p=parent_;auto it=std::find_if(p->children_.begin(),p->children_.end(),[this](auto& x){return x.get()==this;});if(it!=p->children_.end()){p->ChildRemoved.Fire(this);p->children_.erase(it);}parent_=nullptr;}}
std::unique_ptr<Instance> Instance::Clone()const{if(!archivable_)return nullptr;auto c=std::make_unique<Instance>(className_);c->name_=name_;c->archivable_=archivable_;c->attributes_=attributes_;for(auto& x:children_)if(auto y=x->Clone()){y->parent_=c.get();c->children_.push_back(std::move(y));}return c;}
void Instance::SetAttribute(std::string n,AttributeValue v){auto it=attributes_.find(n);if(it!=attributes_.end()&&it->second==v)return;attributes_[n]=std::move(v);AttributeChanged.Fire(n);}
const AttributeValue* Instance::GetAttribute(const std::string& n)const{auto it=attributes_.find(n);return it==attributes_.end()?nullptr:&it->second;}
}

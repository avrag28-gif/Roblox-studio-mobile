#include "instance.h"
#include <algorithm>
namespace rsm {
Instance::Instance(std::string c):className_(std::move(c)){} Instance::~Instance()=default;
void Instance::SetName(std::string n){if(name_==n)return;name_=std::move(n);PropertyChanged.Fire("Name");}
void Instance::SetArchivable(bool v){if(archivable_==v)return;archivable_=v;PropertyChanged.Fire("Archivable");}
void Instance::SetParent(Instance* p){if(p==parent_||p==this)return;if(p){std::unique_ptr<Instance> self;if(parent_){auto* old=parent_;for(auto it=old->children_.begin();it!=old->children_.end();++it)if(it->get()==this){self=std::move(*it);old->children_.erase(it);old->ChildRemoved.Fire(this);break;}}if(!self)return;parent_=nullptr;p->AddChild(std::move(self));}else if(parent_){auto* old=parent_;for(auto it=old->children_.begin();it!=old->children_.end();++it)if(it->get()==this){auto self=std::move(*it);old->children_.erase(it);parent_=nullptr;old->ChildRemoved.Fire(this);break;}}}
Instance* Instance::FindFirstChild(const std::string& n,bool r)const{for(const auto& c:children_){if(c->Name()==n)return c.get();if(r)if(auto* x=c->FindFirstChild(n,true))return x;}return nullptr;}
Instance* Instance::FindFirstChildOfClass(const std::string& c)const{for(const auto& x:children_)if(x->ClassName()==c)return x.get();return nullptr;}
std::vector<Instance*> Instance::GetChildren()const{std::vector<Instance*> r;r.reserve(children_.size());for(auto&c:children_)r.push_back(c.get());return r;}
std::vector<Instance*> Instance::GetDescendants()const{std::vector<Instance*>r;for(auto&c:children_){r.push_back(c.get());auto d=c->GetDescendants();r.insert(r.end(),d.begin(),d.end());}return r;}
void Instance::AddChild(std::unique_ptr<Instance> c){if(!c||c.get()==this)return;if(c->parent_)c->Destroy();c->parent_=this;auto* raw=c.get();children_.push_back(std::move(c));ChildAdded.Fire(raw);}
void Instance::Destroy(){if(destroyed_)return;Destroying.Fire();destroyed_=true;while(!children_.empty())children_.back()->Destroy();if(parent_){auto* old=parent_;for(auto it=old->children_.begin();it!=old->children_.end();++it)if(it->get()==this){old->ChildRemoved.Fire(this);old->children_.erase(it);break;}parent_=nullptr;}}
std::unique_ptr<Instance> Instance::Clone()const{if(!archivable_)return nullptr;auto c=std::make_unique<Instance>(className_);c->name_=name_;c->archivable_=archivable_;c->attributes_=attributes_;for(const auto&x:children_)if(auto d=x->Clone())c->AddChild(std::move(d));return c;}
void Instance::SetAttribute(std::string n,AttributeValue v){auto i=attributes_.find(n);if(i!=attributes_.end()&&i->second==v)return;attributes_[n]=std::move(v);AttributeChanged.Fire(n);}
const AttributeValue* Instance::GetAttribute(const std::string&n)const{auto i=attributes_.find(n);return i==attributes_.end()?nullptr:&i->second;}
}
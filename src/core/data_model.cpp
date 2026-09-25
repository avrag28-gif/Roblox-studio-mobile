#include "data_model.h"
#include "class_system.h"
namespace rsm{
DataModel::DataModel():Instance("DataModel"){SetName("game");InitializeDefaultServices();}
void DataModel::InitializeDefaultServices(){if(!services_.empty())return;const char*n[]={"Workspace","Players","Lighting","ReplicatedStorage","ServerScriptService","ServerStorage","StarterGui","StarterPack","SoundService"};for(auto*s:n){auto x=std::make_unique<Service>(s);auto*r=x.get();AddChild(std::move(x));services_[s]=r;}}
Instance*DataModel::GetService(const std::string&n){auto i=services_.find(n);return i==services_.end()?nullptr:i->second;}
const Instance*DataModel::GetService(const std::string&n)const{auto i=services_.find(n);return i==services_.end()?nullptr:i->second;}
std::unique_ptr<Instance>DataModel::Clone()const{auto c=std::make_unique<DataModel>();c->SetName(Name());c->SetArchivable(Archivable());for(auto* child:GetChildren()){auto copy=child->Clone();Instance::SetParent(std::move(copy),c.get());}return c;}
}
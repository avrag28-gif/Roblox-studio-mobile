#include "data_model.h"
#include "class_system.h"
#include <array>
#include <sstream
#include <vector>
namespace rsm{

DataModel::DataModel():Instance("DataModel"){
 SetName("game");
 InitializeDefaultServices();
}

void DataModel::InitializeDefaultServices(){
 if(!services_.empty())return;
 auto add=[this](std::unique_ptr<Instance> service){
  auto*raw=service.get();
  services_[raw->Name()]=raw;
  Instance::SetParent(std::move(service),this);
 };
 add(std::make_unique<Workspace>());
 add(std::make_unique<Players>());
 add(std::make_unique<Lighting>());
 add(std::make_unique<ReplicatedFirst>());
 add(std::make_unique<ReplicatedStorage>());
 add(std::make_unique<ServerScriptService>());
 add(std::make_unique<ServerStorage>());
 add(std::make_unique<StarterGui>());
 add(std::make_unique<StarterPack>());
 add(std::make_unique<StarterPlayer>());
 add(std::make_unique<SoundService>());
}

Instance*DataModel::ResolvePath(const std::string&path){
 if(path.empty()||path=="game")return this;
 std::string token; Instance*current=this; std::istringstream in(path);
 while(std::getline(in,token,'.')){
  if(token.empty()||token=="game")continue;
  if(current==this){current=GetService(token);}
  else current=current->FindFirstChild(token,false);
  if(!current)return nullptr;
 }
 return current;
}
const Instance*DataModel::ResolvePath(const std::string&path)const{
 if(path.empty()||path=="game")return this;
 std::string token; const Instance*current=this; std::istringstream in(path);
 while(std::getline(in,token,'.')){
  if(token.empty()||token=="game")continue;
  if(current==this)current=GetService(token);
  else current=current->FindFirstChild(token,false);
  if(!current)return nullptr;
 }
 return current;
}

Instance*DataModel::GetService(const std::string&n){
 InitializeDefaultServices();
 auto i=services_.find(n);
 return i==services_.end()?nullptr:i->second;
}
const Instance*DataModel::GetService(const std::string&n)const{
 auto i=services_.find(n);
 return i==services_.end()?nullptr:i->second;
}

void DataModel::ReplaceContentsFrom(const DataModel&source){
 InitializeDefaultServices();
 for(const auto&entry:services_){
  auto*dst=entry.second;
  const auto*src=source.GetService(entry.first);
  if(!src)continue;
  for(auto*child:dst->GetChildren())child->Destroy();
  for(auto*child:src->GetChildren())if(auto copy=child->Clone())Instance::SetParent(std::move(copy),dst);
 }
}

std::unique_ptr<Instance>DataModel::Clone()const{
 auto c=std::make_unique<DataModel>();
 c->SetName(Name());
 c->SetArchivable(Archivable());
 for(const auto&entry:services_){
  auto*dst=c->GetService(entry.first);
  if(!dst)continue;
  const auto*src=entry.second;
  dst->SetArchivable(src->Archivable());
  for(auto*child:src->GetChildren())if(auto copy=child->Clone())Instance::SetParent(std::move(copy),dst);
 }
 return c;
}
}

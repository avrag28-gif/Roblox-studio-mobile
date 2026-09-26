#include "../src/core/data_model.h"
#include "../src/core/instance_factory.h"
#include "../src/core/property_access.h"
#include <cassert>
int main(){
 rsm::DataModel game; auto p=rsm::InstanceFactory::New("Part"); auto* part=dynamic_cast<rsm::BasePart*>(p.get()); assert(part);
 assert(rsm::Instance::SetParent(std::move(p),game.GetService("Workspace")));
 rsm::PropertyValue v; assert(rsm::PropertyAccess::Get(*part,"Position",v)); assert(std::holds_alternative<rsm::Vector3>(v));
 assert(rsm::PropertyAccess::Set(*part,"Position",rsm::Vector3{1,2,3})); assert(part->Position().x==1);
 assert(rsm::PropertyAccess::Set(*part,"Anchored",true)); assert(part->Anchored());
 assert(rsm::PropertyAccess::Set(*part,"Transparency",0.5)); assert(part->Transparency()==0.5f);
 return 0;
}
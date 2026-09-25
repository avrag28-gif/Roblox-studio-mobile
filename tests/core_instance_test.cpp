#include "core/data_model.h"
#include "core/instance_factory.h"
#include <cassert>
int main(){using namespace rsm;DataModel g;auto*w=g.GetService("Workspace");bool hit=false;auto id=w->PropertyChanged.Connect([&](const std::string&n){hit=n=="Name";});w->SetName("World");assert(hit);w->PropertyChanged.Disconnect(id);auto p=InstanceFactory::New("Part");auto*r=Instance::SetParent(std::move(p),w);assert(r&&r->ClassName()=="Part");r->SetName("A");auto c=r->Clone();assert(c&&c->ClassName()=="Part");r->Destroy();assert(!w->FindFirstChild("A"));}
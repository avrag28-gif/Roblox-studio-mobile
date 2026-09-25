#include "../src/core/data_model.h"
#include "../src/core/instance_factory.h"
#include <cassert>
int main(){using namespace rsm;DataModel g;auto*w=g.GetService("Workspace");assert(w);bool hit=false;auto connection=w->PropertyChanged.Connect([&](const std::string&n){hit=n=="Name";});w->SetName("World");assert(hit);connection.Disconnect();auto p=InstanceFactory::New("Part");auto*r=Instance::SetParent(std::move(p),w);assert(r&&r->ClassName()=="Part");r->SetName("A");auto c=r->Clone();assert(c&&c->ClassName()=="Part");r->Destroy();assert(!w->FindFirstChild("A"));return 0;}

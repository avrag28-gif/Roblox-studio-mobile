#include "core/data_model.h"
#include "core/instance_factory.h"
#include <cassert>
int main(){using namespace rsm;DataModel g;auto*w=g.GetService("Workspace");auto p=InstanceFactory::New("Part");auto*r=Instance::SetParent(std::move(p),w);assert(r);assert(w->FindFirstChild("Instance"));}
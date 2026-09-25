#include "core/data_model.h"
#include "core/instance_factory.h"
#include "scene/scene.h"
#include "renderer/renderer.h"
#include "physics/physics_world.h"
#include "core/serialization.h"
#include <cassert>
#include <iostream>
int main(){using namespace rsm;Scene s;auto* ws=s.Game().GetService("Workspace");auto part=InstanceFactory::New("Part");auto* p=dynamic_cast<Part*>(part.get());assert(p);p->SetName("Block");p->SetAnchored(true);Instance::SetParent(std::move(part),ws);assert(ws->FindFirstChild("Block"));NullRenderer r;assert(r.Initialize());r.Render(s.Game());assert(r.DrawCount()==1);auto text=Serializer::Save(s.Game());assert(text.rfind("RSM1",0)==0);std::cout<<"ENGINE_VERTICAL_SLICE_PASS\n";}
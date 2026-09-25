#include "../src/core/data_model.h"
#include "../src/core/instance_factory.h"
#include "../src/renderer/render_world.h"
#include "../src/physics/physics_world.h"
#include "../src/gui/selection.h"
#include "../src/assets/asset_registry.h"
#include "../src/network/network_world.h"
#include "../src/serialization/project_format.h"
#include <cassert>
#include <cstdio>
int main(){using namespace rsm;DataModel game;auto*w=game.GetService("Workspace");assert(w);int added=0;auto c=w->ChildAdded.Connect([&](Instance*){++added;});auto p=InstanceFactory::New("Part");auto*part=Instance::SetParent(std::move(p),w);assert(part&&part->ClassName()=="Part");assert(added==1);auto*bp=dynamic_cast<BasePart*>(part);assert(bp);bp->SetPosition({0,10,0});bp->SetSize({2,2,2});bp->SetAnchored(false);RenderWorld rw;rw.Build(game);assert(rw.VisibleCount()==1);PhysicsWorld pw;pw.Step(.1f,game);assert(bp->Position().y<10);Selection s;s.Set(bp);assert(s.Get()==bp);auto clone=game.Clone();assert(clone);AssetRegistry ar;assert(ar.Register({"mesh://1","a.obj","A",AssetType::Mesh}));NetworkWorld nw;nw.Send({1,{1,2,3}});Packet q;assert(nw.Receive(q)&&q.data.size()==3);int v=0;assert(ProjectFormat::Save(game,"rsm_test.project"));assert(ProjectFormat::LoadHeader("rsm_test.project",v)&&v==ProjectFormat::Version);std::remove("rsm_test.project");(void)c;return 0;}
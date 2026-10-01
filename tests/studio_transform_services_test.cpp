#include "../src/editor/studio_runtime.h"
#include "../src/core/data_model.h"
#include "../src/core/transform_hierarchy.h"
#include <cassert>
#include <cmath>
using namespace rsm;
static bool near(float a,float b){return std::fabs(a-b)<1e-4f;}
int main(){
 DataModel game;
 const char*services[]={"Workspace","Players","Lighting","ReplicatedFirst","ReplicatedStorage","ServerScriptService","ServerStorage","StarterGui","StarterPack","StarterPlayer","SoundService"};
 for(auto name:services){auto*s=game.GetService(name);assert(s);assert(s->Name()==name);assert(s->ClassName()==name);}
 auto*p=InstanceFactory::New("Part");p->SetName("Block");
 auto*raw=p.get();Instance::SetParent(std::move(p),game.Workspace());
 assert(game.ResolvePath("Workspace.Block")==raw);
 assert(game.ResolvePath("game.Workspace.Block")==raw);
 assert(game.FindById(raw->Id())==raw);

 StudioRuntime studio;
 assert(studio.Game().Workspace()!=nullptr);
 assert(studio.CreatePart());
 auto*created=studio.Game().Workspace()->GetChildren().back();
 studio.Selection().Select(created);
 const auto start=WorldPosition(*created);
 studio.Gizmo().SetMode(GizmoMode::Move);studio.Gizmo().SetAxis(GizmoAxis::X);studio.Gizmo().SetSpace(TransformSpace::World);
 studio.ApplyGizmo({3,0,0});
 assert(near(WorldPosition(*created).x,start.x+3));
 assert(studio.CanUndo());
 assert(studio.Undo());
 assert(near(WorldPosition(*created).x,start.x));
 assert(studio.CanRedo());
 assert(studio.Redo());
 assert(near(WorldPosition(*created).x,start.x+3));

 auto p2=InstanceFactory::New("Part");p2->SetName("Second");p2->SetPosition({10,0,0});auto*raw2=p2.get();Instance::SetParent(std::move(p2),studio.Game().Workspace());
 studio.Selection().Select(created);studio.Selection().Add(raw2);
 studio.Gizmo().SetMode(GizmoMode::Move);studio.Gizmo().SetAxis(GizmoAxis::X);studio.Gizmo().SetSpace(TransformSpace::World);
 const auto a0=WorldPosition(*created),b0=WorldPosition(*raw2);
 studio.ApplyGizmo({2,0,0});
 assert(near(WorldPosition(*created).x,a0.x+2)&&near(WorldPosition(*raw2).x,b0.x+2));
 assert(studio.Undo());
 assert(near(WorldPosition(*created).x,a0.x)&&near(WorldPosition(*raw2).x,b0.x));

 Model parentA,parentB;
 parentA.SetPivot(CFrame({10,0,0}));
 parentB.SetPivot(CFrame({100,0,0}));
 auto child=InstanceFactory::New("Part");auto*childRaw=child.get();childRaw->SetCFrame(CFrame({12,0,0}));Instance::SetParent(std::move(child),&parentA);
 studio.Selection().Select(childRaw);
 assert(studio.ReparentSelected(&parentB));
 assert(near(WorldPosition(*childRaw).x,12));
 assert(studio.Undo());
 assert(childRaw->Parent()==&parentA);
 assert(near(WorldPosition(*childRaw).x,12));
 assert(studio.Redo());
 assert(childRaw->Parent()==&parentB);
 assert(near(WorldPosition(*childRaw).x,12));
 return 0;
}

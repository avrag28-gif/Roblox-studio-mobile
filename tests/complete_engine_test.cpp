#include "../src/core/data_model.h"
#include "../src/core/instance_factory.h"
#include "../src/core/property_registry.h"
#include "../src/editor/transform_controller.h"
#include "../src/renderer/render_world.h"
#include "../src/physics/physics_world.h"
#include "../src/runtime/project_store.h"
#include "../src/performance/performance_runtime.h"
#include "../src/security/sandbox.h"
#include "../src/security/credential_store.h"
#include "../src/network/replication_runtime.h"
#include "../src/animation/animation_runtime2.h"
#include <cassert>
#include <iostream>
using namespace rsm;
int main(){
 DataModel game;game.InitializeDefaultServices();auto* ws=game.GetService("Workspace");assert(ws);
 auto p=InstanceFactory::New("Part");p->SetName("TestPart");auto* raw=p.get();Instance::SetParent(std::move(p),ws);assert(ws->FindFirstChild("TestPart"));
 PropertyRegistry::Instance().RegisterBuiltins();assert(PropertyRegistry::Instance().For("Part"));
 auto* part=dynamic_cast<BasePart*>(raw);assert(part);part->SetPosition({1,3,0});part->SetSize({2,2,2});
 PhysicsWorld physics;physics.Step(.016f,game);RenderWorld render;render.Build(game);assert(render.VisibleCount()==1);
 TransformController gizmo;gizmo.Begin(part,TransformMode::Move,GizmoAxis::X);gizmo.Apply(1,0);gizmo.End();assert(part->Position().x>1);
 ScriptSandbox sandbox;std::string err;assert(sandbox.ValidateSource("print('ok')",err));assert(!sandbox.ValidateSource("io.open('x')",err));
 CredentialStore creds;assert(creds.Put("session","opaque"));assert(creds.Get("session")=="opaque");creds.Clear();
 ReplicationRuntime repl;repl.Queue("Workspace.TestPart","Position","2,3,0");ReplicationMessage m;assert(repl.Pop(m));assert(repl.ApplyOrdered(m,[](const auto&){return true;}));
 AnimationTrack track;track.Add({0,{0,0,0}});track.Add({1,{10,0,0}});track.Play();track.Step(.5f);assert(track.Sample().x>0&&track.Sample().x<10);
 PerformanceRuntime perf;perf.BeginFrame();perf.SetDrawCalls(render.VisibleCount());perf.SetMemory(1024);perf.EndFrame();assert(perf.Frame().drawCalls==1);
 auto json=ProjectStore::Save(game);assert(json.find("\"version\"")!=std::string::npos);
 auto clone=game.Clone();assert(clone&&dynamic_cast<DataModel*>(clone.get()));std::cout<<"all vertical systems ok\n";
}
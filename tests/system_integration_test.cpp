#include "../src/engine/engine_system.h"
#include "../src/project/project_store.h"
#include "../src/core/base_part.h"
#include <cassert>
#include <string>
using namespace rsm;
int main(){
 DataModel game; auto ws=game.GetService("Workspace"); assert(ws);
 auto p=std::make_unique<Part>(); p->SetName("Part"); p->SetPosition({1,2,3}); p->SetAnchored(false); Instance::SetParent(std::move(p),ws);
 assert(ws->FindFirstChild("Part"));
 EngineSystem e; e.physics.Sync(*ws); assert(e.physics.BodyCount()==1); e.physics.Step(0.016f);
 std::string log; e.scripts=LuauService([&](std::string s){log=s;}); assert(e.scripts.CompileAndRun("print('ok')")); assert(!log.empty());
 e.sandbox.Allow("Instance"); std::string err; assert(e.sandbox.ValidateSource("local x=1",err)); assert(!e.sandbox.ValidateSource("os.execute()",err));
 assert(ProjectStore::Valid(ProjectStore::Serialize(game)));
 assert(e.assets.Import("builtin:part","builtin:part",ImportType::Mesh));
 e.audio.Play("click"); assert(e.audio.Get("click")->playing); e.audio.SetVolume("click",2); assert(e.audio.Get("click")->volume==1);
 AnimationTrack t; t.keys={{0,{0,0,0},{},{1,1,1}},{1,{10,0,0},{},{1,1,1}}}; e.animation.SetTrack(t); e.animation.Step(.5f); assert(e.animation.Sample().position.x==5);
 assert(e.runtime.Server().Role()==RuntimeRole::Server); e.runtime.Replication().Send("SetProperty","Position"); RpcMessage m; assert(e.runtime.Replication().Receive(m)&&m.method=="SetProperty");
 assert(e.runtime.Replication().Pending()==0);
 return 0;
}
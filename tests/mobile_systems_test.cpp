#include "assets/mesh_importer.h"
#include "audio/audio_world.h"
#include "editor/gizmo_controller.h"
#include "editor/selection_service.h"
#include "network/ownership.h"
#include "runtime/network_runtime.h"
#include "performance/resource_budget.h"
#include "platform/android_lifecycle.h"
#include "project/project_manifest.h"
#include "runtime/script_scheduler.h"
#include "security/project_validator.h"
#include "serialization/scene_codec.h"
#include <cassert>
#include <iostream>
using namespace rsm;
int main(){
 Mesh m;std::string e;assert(MeshImporter::OBJ("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n",m,e));assert(m.indices.size()==3);
 Part p;p.SetPosition({1,2,3});GizmoController g;g.SetMode(GizmoMode::Move);g.SetAxis(GizmoAxis::X);g.Apply(p,{2,4,6});assert(p.Position().x==3&&p.Position().y==2);
 OwnershipTable own;own.Set(7,4);assert(own.CanWrite(7,4)&&!own.CanWrite(7,5));
 ResourceUsage u;u.Set(1,1,1,1);assert(u.Within(ResourceBudget{}));
 AndroidLifecycle l;l.Start();l.SurfaceCreated();l.Pause();assert(l.State()==AppState::Paused&&l.HasSurface());l.Resume();assert(l.State()==AppState::Started);
 ProjectManifest pm;pm.name="Demo";auto enc=ProjectManifestCodec::Encode(pm);ProjectManifest pm2;assert(ProjectManifestCodec::Decode(enc,pm2)&&pm2.name=="Demo");
 ScriptScheduler s;int ran=0;s.Enqueue([&]{++ran;});s.Step();assert(ran==1);
 assert(ProjectValidator::SafeRelativePath("assets/a.mesh"));assert(!ProjectValidator::SafeRelativePath("../secret"));DataModel dm;auto part=InstanceFactory::New("Part");part->SetName("SerializedPart");Instance::SetParent(std::move(part),dm.GetService("Workspace"));assert(SceneCodec::Valid(SceneCodec::Save(dm)));
 AudioWorld aw;auto id=aw.Create({0,0,0});aw.Play(id);assert(aw.Gain(id,{0,0,0})>0);
 std::cout<<"mobile systems ok\n";return 0;
}
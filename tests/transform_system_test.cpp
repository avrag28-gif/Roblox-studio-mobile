#include "../src/core/transform_hierarchy.h"
#include "../src/core/world_bounds.h"
#include "../src/core/model_bounds.h"
#include "../src/editor/transform_controller.h"
#include "../src/editor/gizmo_controller.h"
#include "../src/physics/physics_world.h"
#include <cassert>
#include <cmath>
using namespace rsm;
static bool near(float a,float b){return std::fabs(a-b)<1e-4f;}
int main(){
 const float pi=3.14159265358979323846f;

 Model parent;
 parent.SetPivot(CFrame({10,2,3},Quaternion::FromAxisAngle({0,1,0},pi/2)));
 auto part=std::make_unique<Part>();
 part->SetCFrame(CFrame({12,2,3},Quaternion::FromAxisAngle({0,1,0},.25f)));
 auto* p=part.get();
 Instance::SetParent(std::move(part),&parent);

 auto local=LocalCFrame(*p);
 assert(near(local.position.x,2));
 assert(near(local.position.y,0));
 assert(near(local.position.z,0));

 const auto before=WorldCFrame(*p);
 SetLocalCFrame(*p,CFrame({4,5,6},Quaternion::FromAxisAngle({1,0,0},.4f)));
 const auto after=WorldCFrame(*p);
 assert(near(after.position.x,6));
 assert(near(after.position.y,7));
 assert(near(after.position.z,-1));
 assert(!near(before.rotation.x,after.rotation.x));

 const auto preserved=WorldCFrame(*p);
 Model other; other.SetPivot(CFrame({100,0,0}));
 ReparentPreserveWorld(*p,other);
 const auto rep=WorldCFrame(*p);
 assert(near(rep.position.x,preserved.position.x)&&near(rep.position.y,preserved.position.y)&&near(rep.position.z,preserved.position.z));
 assert(near(rep.rotation.x,preserved.rotation.x)&&near(rep.rotation.y,preserved.rotation.y)&&near(rep.rotation.z,preserved.rotation.z)&&near(rep.rotation.w,preserved.rotation.w));

 TransformController tc;
 tc.Begin(p,TransformMode::Move,GizmoAxis::X,TransformSpace::Local);
 auto start=WorldPosition(*p);
 tc.Apply(1,0);tc.End();
 auto moved=WorldPosition(*p);
 auto expectedDelta=WorldRotation(*p).Rotate({1,0,0});
 assert(near(moved.x-start.x,expectedDelta.x)&&near(moved.y-start.y,expectedDelta.y)&&near(moved.z-start.z,expectedDelta.z));

 tc.Begin(p,TransformMode::Rotate,GizmoAxis::Y,TransformSpace::World);
 const auto r0=WorldRotation(*p);
 tc.Apply(1,0);tc.End();
 const auto r1=WorldRotation(*p);
 assert(!near(r0.x,r1.x)||!near(r0.y,r1.y)||!near(r0.z,r1.z)||!near(r0.w,r1.w));

 GizmoController gizmo;
 gizmo.SetMode(GizmoMode::Move);gizmo.SetAxis(GizmoAxis::Y);gizmo.SetSpace(TransformSpace::World);
 auto g0=WorldPosition(*p);gizmo.Apply(*p,{4,2,7});auto g1=WorldPosition(*p);
 assert(near(g1.x,g0.x)&&near(g1.y,g0.y+2)&&near(g1.z,g0.z));

 p->SetSize({2,4,6});
 p->SetCFrame(CFrame(WorldPosition(*p),Quaternion::FromAxisAngle({0,1,0},pi/2)));
 auto b=WorldBounds(*p);
 assert(near(b.max.x-b.min.x,6)&&near(b.max.y-b.min.y,4)&&near(b.max.z-b.min.z,2));

 PhysicsWorld physics;
 Ray ray{b.min,{1,0,0}};
 auto hit=physics.Raycast(parent,ray);
 (void)hit;

 Model boundsModel;
 auto bp=std::make_unique<Part>();
 bp->SetSize({2,4,6});
 bp->SetCFrame(CFrame({5,0,0},Quaternion::FromAxisAngle({0,1,0},pi/2)));
 Instance::SetParent(std::move(bp),&boundsModel);
 auto ext=ModelWorldAabbSize(boundsModel);
 assert(near(ext.x,6)&&near(ext.y,4)&&near(ext.z,2));
 return 0;
}

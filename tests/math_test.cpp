#include "../src/math/math3d.h"
#include "../src/math/cframe.h"
#include "../src/core/base_part.h"
#include "../src/core/class_system.h"
#include "../src/core/transform_hierarchy.h"
#include "../src/core/model_bounds.h"
#include <cassert>
#include <cmath>
using namespace rsm;
static bool near(float a,float b){return std::fabs(a-b)<1e-4f;}
int main(){
 Vector3 a{1,2,3},b{2,1,0};auto c=a+b;assert(c.x==3&&c.y==3&&c.z==3);assert(Vector3::dot(a,b)==4);auto n=a.normalized();assert(n.length()>0.99f&&n.length()<1.01f);
 auto parent=CFrame({10,0,0},Quaternion::FromAxisAngle({0,1,0},3.14159265358979323846f/2));
 auto child=CFrame({2,0,0}); auto world=parent*child; assert(near(world.position.x,10)); assert(near(world.position.z,-2));
 auto local=parent.Inverse()*world; assert(near(local.position.x,2)); assert(near(local.position.y,0)); assert(near(local.position.z,0));
 auto pworld=parent.PointToWorldSpace({0,0,-2}); assert(near(pworld.x,8)); assert(near(pworld.z,0));
 auto back=parent.PointToObjectSpace(pworld); assert(near(back.x,0)); assert(near(back.z,-2));

 Model model;
 model.SetPivot(CFrame({5,0,0}));
 auto part=std::make_unique<Part>();
 part->SetCFrame(CFrame({7,0,0}));
 auto*partRaw=part.get();
 Instance::SetParent(std::move(part),&model);
 ApplyModelWorldPivot(model,CFrame({10,0,0}));
 assert(near(model.Pivot().position.x,10));
 assert(near(partRaw->Position().x,12));

 auto nested=std::make_unique<Model>();
 nested->SetPivot(CFrame({12,0,0}));
 auto*nestedRaw=nested.get();
 Instance::SetParent(std::move(nested),&model);
 auto nestedPart=std::make_unique<Part>();
 nestedPart->SetPosition({13,0,0});
 auto*nestedPartRaw=nestedPart.get();
 Instance::SetParent(std::move(nestedPart),nestedRaw);
 ApplyModelWorldPivot(model,CFrame({8,0,0}));
 assert(near(nestedRaw->Pivot().position.x,10));
 assert(near(nestedPartRaw->Position().x,11));

 // Nested model rotation and translation must propagate to its parts exactly once.
 Model rotated;
 rotated.SetPivot(CFrame({0,0,0},Quaternion::FromAxisAngle({0,1,0},3.14159265358979323846f/2)));
 auto rp=std::make_unique<Part>(); rp->SetCFrame(CFrame({2,0,0})); auto*rpRaw=rp.get();
 Instance::SetParent(std::move(rp),&rotated);
 rotated.SetPivot(CFrame({5,0,0},Quaternion::FromAxisAngle({0,1,0},3.14159265358979323846f/2)));
 assert(near(rpRaw->Position().x,5)); assert(near(rpRaw->Position().z,-2));

 Model boundsModel;
 auto boundsPart=std::make_unique<Part>();
 boundsPart->SetSize({2,4,6});
 boundsPart->SetCFrame(CFrame({5,0,0},Quaternion::FromAxisAngle({0,1,0},3.14159265358979323846f/2)));
 Instance::SetParent(std::move(boundsPart),&boundsModel);
 auto bounds=boundsModel;
 const auto extents=ModelWorldAabbSize(bounds);
 assert(near(extents.x,6)); assert(near(extents.y,4)); assert(near(extents.z,2));

 Model target;
 target.SetPivot(CFrame({20,0,0}));
 auto reparented=std::make_unique<Part>();
 reparented->SetCFrame(CFrame({3,4,5},Quaternion::FromAxisAngle({0,1,0},0.4f)));
 auto*reparentedRaw=reparented.get();
 Instance::SetParent(std::move(reparented),&model);
 const CFrame before=WorldCFrame(*reparentedRaw);
 ReparentPreserveWorld(*reparentedRaw,target);
 const CFrame after=WorldCFrame(*reparentedRaw);
 assert(near(before.position.x,after.position.x));
 assert(near(before.position.y,after.position.y));
 assert(near(before.position.z,after.position.z));
 assert(near(before.rotation.x,after.rotation.x));
 assert(near(before.rotation.y,after.rotation.y));
 assert(near(before.rotation.z,after.rotation.z));
 assert(near(before.rotation.w,after.rotation.w));
 return 0;
}

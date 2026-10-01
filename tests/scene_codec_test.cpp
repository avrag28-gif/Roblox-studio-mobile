#include "../src/serialization/scene_codec.h"
#include "../src/core/instance_factory.h"
#include <cassert>
#include <cmath>
#include <string>
int main(){
 using namespace rsm; DataModel source; auto* ws=source.GetService("Workspace"); assert(ws);
 auto model=InstanceFactory::New("Model"); model->SetName("TestModel"); model->SetId("model-stable-id"); auto* mm=dynamic_cast<Model*>(model.get()); assert(mm); mm->SetPivot(CFrame({10,20,30},Quaternion::FromEulerXYZ(.1f,.2f,.3f))); auto* mr=Instance::SetParent(std::move(model),ws); assert(mr);
 auto part=InstanceFactory::New("Part"); part->SetName("TestPart"); part->SetId("part-stable-id"); auto* p=dynamic_cast<BasePart*>(part.get()); assert(p);
 p->SetPosition({1,2,3}); p->SetSize({4,5,6}); p->SetColor({.1f,.2f,.3f}); p->SetTransparency(.25f); p->SetAnchored(true); p->SetCanCollide(false); p->SetCanTouch(false); p->SetCanQuery(true); p->SetMass(7.5f); p->SetShape(PartShape::Cylinder); p->SetMaterial(Material::Metal);
 p->SetAttribute("Bool",true); p->SetAttribute("Number",42.5); p->SetAttribute("Text",std::string("hello|world")); Instance::SetParent(std::move(part),mr);
 auto script=InstanceFactory::New("Script"); script->SetName("ServerScript"); script->SetId("script-stable-id"); auto* sr=dynamic_cast<Script*>(script.get()); assert(sr); sr->SetSource("print('round trip')"); Instance::SetParent(std::move(script),mr);
 auto encoded=SceneCodec::Save(source); assert(encoded.rfind("RSM_SCENE 2\n",0)==0); std::string error; auto loaded=SceneCodec::Load(encoded,error); assert(loaded&&error.empty());
 auto* lws=loaded->GetService("Workspace"); auto* lm=lws->FindFirstChild("TestModel"); assert(lm&&lm->Id()=="model-stable-id"); auto* lmm=dynamic_cast<Model*>(lm); assert(lmm); assert(std::fabs(lmm->Pivot().position.x-10)<1e-5); auto le=lmm->Pivot().rotation.ToEulerXYZ(); assert(std::fabs(le.y-.2f)<1e-5);
 auto* lp=dynamic_cast<BasePart*>(lm->FindFirstChild("TestPart")); assert(lp&&lp->Id()=="part-stable-id"); assert(std::fabs(lp->Position().x-1)<1e-5); assert(std::fabs(lp->Size().z-6)<1e-5); assert(std::fabs(lp->Color().g-.2f)<1e-5); assert(std::fabs(lp->Transparency()-.25f)<1e-5); assert(lp->Anchored()&&!lp->CanCollide()&&!lp->CanTouch()&&lp->CanQuery()); assert(std::fabs(lp->Mass()-7.5f)<1e-5); assert(lp->Shape()==PartShape::Cylinder); assert(lp->MaterialValue()==Material::Metal);
 auto* b=lp->GetAttribute("Bool"); auto* n=lp->GetAttribute("Number"); auto* t=lp->GetAttribute("Text"); assert(b&&std::get<bool>(*b)); assert(n&&std::fabs(std::get<double>(*n)-42.5)<1e-9); assert(t&&std::get<std::string>(*t)=="hello|world");
 auto* ls=dynamic_cast<Script*>(lm->FindFirstChild("ServerScript")); assert(ls&&ls->Id()=="script-stable-id"&&ls->Source()=="print('round trip')"); return 0;
}
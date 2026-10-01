#include "../src/editor/studio_runtime.h"
#include <cassert>
int main(){
 rsm::StudioRuntime studio;
 auto*ws=studio.Game().GetService("Workspace"); assert(ws);
 const bool created=studio.CreatePart(); assert(created);
 auto children=ws->GetChildren(); assert(children.size()==1);
 auto*p=dynamic_cast<rsm::BasePart*>(children[0]); assert(p);
 studio.Selection().Select(p);
 const bool propertySet=studio.SetProperty("Position",rsm::Vector3{3,4,5}); assert(propertySet);
 assert(p->Position().x==3&&p->Position().y==4&&p->Position().z==5);
 studio.Gizmo().SetMode(rsm::GizmoMode::Move);
 studio.Gizmo().SetAxis(rsm::GizmoAxis::X);
 studio.ApplyGizmo({2,9,9});
 assert(p->Position().x==5&&p->Position().y==4&&p->Position().z==5);
 const bool duplicated=studio.DuplicateSelected(); assert(duplicated);
 assert(ws->GetChildren().size()==2);
 const bool played=studio.Play(); assert(played); assert(studio.Playing());
 studio.Stop(); assert(!studio.Playing());
 assert(studio.Render().VisibleCount()==2);
 return 0;
}

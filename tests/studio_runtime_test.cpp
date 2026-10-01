#include "../src/editor/studio_runtime.h"
#include <cassert>
#include <iostream>
int main(){std::cerr<<"start\n";
 rsm::StudioRuntime studio;
 auto*ws=studio.Game().GetService("Workspace"); std::cerr<<"workspace\n"; assert(ws);
 std::cerr<<"before create\n";assert(studio.CreatePart());std::cerr<<"created\n";
 auto children=ws->GetChildren(); assert(children.size()==1);
 auto*p=dynamic_cast<rsm::BasePart*>(children[0]); assert(p);
 studio.Selection().Select(p);std::cerr<<"selected\n";
 std::cerr<<"before property\n";assert(studio.SetProperty("Position",rsm::Vector3{3,4,5}));std::cerr<<"property\n";
 assert(p->Position().x==3&&p->Position().y==4&&p->Position().z==5);
 std::cerr<<"before gizmo\n";studio.Gizmo().SetMode(rsm::GizmoMode::Move); studio.Gizmo().SetAxis(rsm::GizmoAxis::X); studio.ApplyGizmo({2,9,9});
 assert(p->Position().x==5&&p->Position().y==4&&p->Position().z==5);std::cerr<<"gizmo done\n";
 assert(studio.DuplicateSelected());std::cerr<<"duplicated\n"; assert(ws->GetChildren().size()==2);
 std::cerr<<"before play\n";assert(studio.Play());std::cerr<<"after play\n"; assert(studio.Playing()); studio.Stop();std::cerr<<"stopped\n"; assert(!studio.Playing());
 assert(studio.Render().VisibleCount()==2);
 return 0;
}
#pragma once
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../core/property_access.h"
#include "../editor/selection_service.h"
#include "../editor/gizmo_controller.h"
#include "../gui/explorer_controller.h"
#include "../gui/property_panel.h"
#include "../gui/script_editor_model.h"
#include "../gui/output_console.h"
#include "../renderer/render_world.h"
#include "../runtime/play_session.h"
#include <string>
#include <vector>
namespace rsm {
class StudioRuntime {
 DataModel editor_;
 SelectionService selection_;
 GizmoController gizmo_;
 ExplorerController explorer_;
 RenderWorld renderWorld_;
 PlaySession play_;
 OutputConsole output_;
 ScriptEditorModel script_;
 bool playing_=false;
public:
 StudioRuntime(){explorer_.Bind(&editor_);RebuildRenderWorld();}
 DataModel&Game(){return editor_;}
 const DataModel&Game()const{return editor_;}
 ExplorerController&Explorer(){return explorer_;}
 SelectionService&Selection(){return selection_;}
 GizmoController&Gizmo(){return gizmo_;}
 RenderWorld&Render(){return renderWorld_;}
 OutputConsole&Output(){return output_;}
 ScriptEditorModel&ScriptEditor(){return script_;}
 bool CreatePart(Instance*parent=nullptr){
   auto p=InstanceFactory::New("Part"); if(!p)return false;
   if(!Instance::SetParent(std::move(p),parent?parent:editor_.GetService("Workspace")))return false;
   RebuildRenderWorld(); return true;
 }
 bool DeleteSelected(){
   auto*i=selection_.Selected(); if(!i||i==&editor_)return false;
   i->Destroy();selection_.Clear();RebuildRenderWorld();return true;
 }
 bool DuplicateSelected(){
   auto*i=selection_.Selected();if(!i||!i->Archivable())return false;
   auto c=i->Clone();if(!c)return false;auto*p=i->Parent();if(!p)return false;
   auto*raw=c.get();raw->SetName(i->Name()+" Copy");p->AddChild(std::move(c));selection_.Select(raw);RebuildRenderWorld();return true;
 }
 bool ReparentSelected(Instance*parent){
   auto*i=selection_.Selected();if(!i||!parent||i==parent)return false;
   if(i->IsAncestorOf(parent))return false;i->SetParent(parent);RebuildRenderWorld();return i->Parent()==parent;
 }
 bool SetProperty(const std::string&name,const PropertyValue&value){
   auto*i=selection_.Selected();if(!i)return false;
   bool ok=PropertyAccess::Set(*i,name,value);if(ok)RebuildRenderWorld();return ok;
 }
 void ApplyGizmo(const Vector3&delta){if(auto*p=dynamic_cast<BasePart*>(selection_.Selected())){gizmo_.Apply(*p,delta);RebuildRenderWorld();}}
 bool Play(){
   if(playing_)return false;
   if(!play_.Start(editor_))return false;
   playing_=true;output_.Push(LogLevel::Info,"Play session started");return true;
 }
 void Stop(){
   if(!playing_)return;
   play_.Stop();playing_=false;output_.Push(LogLevel::Info,"Play session stopped");
 }
 bool Playing()const{return playing_;}
 void RebuildRenderWorld(){renderWorld_.Build(editor_);}
 std::vector<PropertyField>SelectedProperties()const{
   if(auto*i=selection_.Selected())return PropertyPanel::Describe(*i);return {};
 }
};
}
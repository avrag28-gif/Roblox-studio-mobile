#pragma once
#include "../core/data_model.h"
#include "../core/instance_factory.h"
#include "../core/property_access.h"
#include "../core/change_history.h"
#include "../core/transform_hierarchy.h"
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
#include <sstream>
#include <cmath>
#include <algorithm>

namespace rsm {

class StudioRuntime {
 struct NumericState { std::string id,property; double value=0; };
 DataModel editor_;
 SelectionService selection_;
 GizmoController gizmo_;
 ExplorerController explorer_;
 RenderWorld renderWorld_;
 PlaySession play_;
 OutputConsole output_;
 ScriptEditorModel script_;
 ChangeHistory history_;
 bool playing_=false;

 Instance* Find(const std::string&id){return editor_.FindById(id);}
 static double ReadTransform(const Instance& i,const std::string& p){
  const CFrame cf=WorldCFrame(i);
  if(p=="PositionX")return cf.position.x;
  if(p=="PositionY")return cf.position.y;
  if(p=="PositionZ")return cf.position.z;
  const auto e=cf.rotation.ToEulerXYZ();
  if(p=="RotationX")return e.x;
  if(p=="RotationY")return e.y;
  if(p=="RotationZ")return e.z;
  if(auto*part=dynamic_cast<const BasePart*>(&i)){
   if(p=="SizeX")return part->Size().x;
   if(p=="SizeY")return part->Size().y;
   if(p=="SizeZ")return part->Size().z;
  }
  return 0;
 }
 bool ApplyTransformValue(const std::string&id,const std::string&p,double value){
  auto*i=Find(id);if(!i)return false;
  auto cf=WorldCFrame(*i);
  if(p=="PositionX")cf.position.x=static_cast<float>(value);
  else if(p=="PositionY")cf.position.y=static_cast<float>(value);
  else if(p=="PositionZ")cf.position.z=static_cast<float>(value);
  else if(p=="RotationX"||p=="RotationY"||p=="RotationZ"){
   auto e=cf.rotation.ToEulerXYZ();
   if(p=="RotationX")e.x=static_cast<float>(value);
   if(p=="RotationY")e.y=static_cast<float>(value);
   if(p=="RotationZ")e.z=static_cast<float>(value);
   cf.rotation=Quaternion::FromEulerXYZ(e.x,e.y,e.z);
  } else if(auto*part=dynamic_cast<BasePart*>(i)){
   auto s=part->Size();
   if(p=="SizeX")s.x=std::max(.05f,static_cast<float>(value));
   else if(p=="SizeY")s.y=std::max(.05f,static_cast<float>(value));
   else if(p=="SizeZ")s.z=std::max(.05f,static_cast<float>(value));
   else return false;
   part->SetSize(s);
   return true;
  } else return false;
  SetWorldCFrame(*i,cf);
  return true;
 }
 std::vector<NumericState> CaptureTransform(Instance&root)const{
  std::vector<NumericState> out;
  std::vector<Instance*> nodes{&root};
  auto descendants=root.GetDescendants();
  nodes.insert(nodes.end(),descendants.begin(),descendants.end());
  for(auto*i:nodes){
   const bool transformable=dynamic_cast<const BasePart*>(i)||dynamic_cast<const Model*>(i);
   if(!transformable)continue;
   for(const char*p:{"PositionX","PositionY","PositionZ","RotationX","RotationY","RotationZ"})
    out.push_back({i->Id(),p,ReadTransform(*i,p)});
   if(dynamic_cast<const BasePart*>(i))
    for(const char*p:{"SizeX","SizeY","SizeZ"})out.push_back({i->Id(),p,ReadTransform(*i,p)});
  }
  return out;
 }
 void RecordTransformDiff(const std::vector<NumericState>&before,const std::vector<NumericState>&after){
  history_.BeginTransaction();
  for(size_t i=0;i<before.size()&&i<after.size();++i)
   if(before[i].id==after[i].id&&before[i].property==after[i].property&&std::fabs(before[i].value-after[i].value)>1e-7)
    history_.Push({before[i].id,before[i].property,before[i].value,after[i].value});
  history_.EndTransaction();
 }

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
  auto p=InstanceFactory::New("Part");if(!p)return false;
  if(!Instance::SetParent(std::move(p),parent?parent:editor_.Workspace()))return false;
  RebuildRenderWorld();return true;
 }
 bool DeleteSelected(){
  auto*i=selection_.Selected();if(!i||i==&editor_)return false;
  i->Destroy();selection_.Clear();RebuildRenderWorld();return true;
 }
 bool DuplicateSelected(){
  auto*i=selection_.Selected();if(!i||!i->Archivable())return false;
  auto c=i->Clone();if(!c)return false;auto*p=i->Parent();if(!p)return false;
  auto*raw=c.get();raw->SetName(i->Name()+" Copy");if(!Instance::SetParent(std::move(c),p))return false;
  selection_.Select(raw);RebuildRenderWorld();return true;
 }
 bool ReparentSelected(Instance*parent){
  auto*i=selection_.Selected();if(!i||!parent||i==parent)return false;
  for(auto*a=parent;a;a=a->Parent())if(a==i)return false;
  const std::string oldParent=i->Parent()?i->Parent()->Id():"";
  const std::string newParent=parent->Id();
  ReparentPreserveWorld(*i,*parent);
  history_.Push({"", "__STRUCTURE__",0,0,oldParent+"|"+newParent+"|"+i->Id()});
  RebuildRenderWorld();
  return i->Parent()==parent;
 }
 bool SetProperty(const std::string&name,const PropertyValue&value){
  auto*i=selection_.Selected();if(!i)return false;
  bool ok=PropertyAccess::Set(*i,name,value);if(ok)RebuildRenderWorld();return ok;
 }
 void ApplyGizmo(const Vector3&delta){
  auto*i=selection_.Selected();if(!i)return;
  auto before=CaptureTransform(*i);
  gizmo_.Apply(*i,delta);
  auto after=CaptureTransform(*i);
  RecordTransformDiff(before,after);
  RebuildRenderWorld();
 }
 bool Undo(){
  const bool ok=history_.UndoAny([this](const std::string&id,const std::string&p,double v){return ApplyTransformValue(id,p,v);},{},[this](const ChangeHistory::Command&c){
   if(c.property!="__STRUCTURE__")return false;
   std::stringstream ss(c.payload);std::string oldParent,newParent,id;
   if(!std::getline(ss,oldParent,'|')||!std::getline(ss,newParent,'|')||!std::getline(ss,id,'|'))return false;
   auto*node=Find(id);auto*parent=oldParent.empty()?static_cast<Instance*>(&editor_):Find(oldParent);
   if(!node||!parent)return false;ReparentPreserveWorld(*node,*parent);RebuildRenderWorld();return true;
  });
  if(ok)RebuildRenderWorld();return ok;
 }
 bool Redo(){
  const bool ok=history_.RedoAny([this](const std::string&id,const std::string&p,double v){return ApplyTransformValue(id,p,v);},{},[this](const ChangeHistory::Command&c){
   if(c.property!="__STRUCTURE__")return false;
   std::stringstream ss(c.payload);std::string oldParent,newParent,id;
   if(!std::getline(ss,oldParent,'|')||!std::getline(ss,newParent,'|')||!std::getline(ss,id,'|'))return false;
   auto*node=Find(id);auto*parent=newParent.empty()?static_cast<Instance*>(&editor_):Find(newParent);
   if(!node||!parent)return false;ReparentPreserveWorld(*node,*parent);RebuildRenderWorld();return true;
  });
  if(ok)RebuildRenderWorld();return ok;
 }
 bool CanUndo()const{return history_.CanUndo();}
 bool CanRedo()const{return history_.CanRedo();}
 bool Play(){
  if(playing_)return false;
  if(!play_.Start(editor_))return false;
  playing_=true;output_.Push(LogLevel::Info,"Play session started");return true;
 }
 void Stop(){if(!playing_)return;play_.Stop();playing_=false;output_.Push(LogLevel::Info,"Play session stopped");}
 bool Playing()const{return playing_;}
 void RebuildRenderWorld(){renderWorld_.Build(editor_);}
 std::vector<PropertyField>SelectedProperties()const{
  if(auto*i=selection_.Selected())return PropertyPanel::Describe(*i);return {};
 }
};

}

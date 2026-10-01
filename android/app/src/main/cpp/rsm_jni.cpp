#include <jni.h>
#include "core/data_model.h"
#include "core/instance_factory.h"
#include "core/change_history.h"
#include "serialization/scene_codec.h"
#include "renderer/gles_renderer.h"
#include "renderer/camera.h"
#include "physics/physics_world.h"
#include "runtime/play_session.h"
#include "scripting/luau_service.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include "platform/android_lifecycle.h"

static rsm::AndroidLifecycle lifecycle;
static rsm::DataModel game;
static std::unique_ptr<rsm::GLESRenderer> renderer;
static rsm::Instance* workspace=nullptr;
static rsm::Camera camera;
static std::string lastScriptLog;
static std::unique_ptr<rsm::DataModel> runtimeGame;
static std::mutex engineMutex;
static std::unordered_map<std::string, rsm::Instance*> editorIndex;
static rsm::PhysicsWorld runtimePhysics;
static rsm::ChangeHistory changeHistory;
static bool applyingHistory=false;

struct StructuralSnapshot {
 std::string id;
 std::string parentId;
 std::unique_ptr<rsm::Instance> tree;
 std::vector<std::pair<const rsm::Instance*,std::string>> ids;
};
static std::unordered_map<std::string,StructuralSnapshot> structuralSnapshots;
static std::uint64_t nextStructuralToken=1;

static void CollectSnapshotIdsWithSource(const rsm::Instance*src,rsm::Instance*copy,const std::string&rootId,std::vector<std::pair<const rsm::Instance*,std::string>>&out){
 if(!src||!copy)return;
 out.push_back({copy,rootId});
 auto sc=src->GetChildren(),cc=copy->GetChildren();
 for(std::size_t i=0;i<sc.size()&&i<cc.size();++i){
  std::string id;
  for(const auto&e:editorIndex)if(e.second==sc[i]){id=e.first;break;}
  if(id.empty())id=rootId+"#child"+std::to_string(i+1);
  CollectSnapshotIdsWithSource(sc[i],cc[i],id,out);
 }
}

static void AssignFreshIds(rsm::Instance*root,const std::string&base){
 if(!root)return;
 root->SetId(base);
 std::size_t i=1;
 for(auto*child:root->GetDescendants()){
  if(child==root)continue;
  child->SetId(base+"#child"+std::to_string(i++));
 }
}
static void IndexSubtree(rsm::Instance*root){
 if(!root)return;
 editorIndex[root->Id()]=root;
 for(auto*x:root->GetDescendants())editorIndex[x->Id()]=x;
}
static std::string MakeStructurePayload(const std::string&op,const std::string&id,const std::string&parent,const std::string&token){
 return op+"|"+id+"|"+parent+"|"+token;
}
static void RecordStructure(const std::string&payload){
 if(!applyingHistory)changeHistory.Push({"","__STRUCTURE__",0,0,payload});
}

static bool IsInSubtree(const rsm::Instance*root,const rsm::Instance*node){
 if(!root||!node)return false;
 if(root==node)return true;
 for(auto*x:root->GetDescendants())if(x==node)return true;
 return false;
}
static rsm::Instance* ResolveParent(const std::string&id){
 if(id.empty())return workspace;
 auto it=editorIndex.find(id);
 return it==editorIndex.end()?nullptr:it->second;
}
static bool RestoreStructural(const rsm::ChangeHistory::Command&cmd,bool undo){
 std::vector<std::string>f;std::string part;std::stringstream ss(cmd.payload);
 while(std::getline(ss,part,'|'))f.push_back(part);
 if(f.size()<4)return false;
 const std::string&op=f[0],&id=f[1],&parentId=f[2],&token=f[3];
 auto sit=structuralSnapshots.find(token);
 if(sit==structuralSnapshots.end())return false;
 if(op=="REPARENT"){
  auto it=editorIndex.find(id);if(it==editorIndex.end())return false;
  rsm::Instance*target=ResolveParent(undo?sit->second.parentId:parentId);
  if(!target||target==it->second||IsInSubtree(it->second,target))return false;
  it->second->SetParent(target);return true;
 }
 rsm::Instance*restoreParent=ResolveParent(parentId);
 if(!restoreParent)return false;
 const bool restore=(op=="DELETE"&&undo)||(op=="CREATE"&&!undo);
 if(restore){
  if(editorIndex.find(id)!=editorIndex.end())return false;
  auto copy=sit->second.tree->Clone();if(!copy)return false;
  rsm::Instance*raw=copy.get();
  rsm::Instance::SetParent(std::move(copy),restoreParent);
  auto ids=sit->second.ids;
  std::vector<rsm::Instance*>restored{raw};
  for(auto*x:raw->GetDescendants())restored.push_back(x);
  if(restored.size()!=ids.size())return false;
  for(std::size_t i=0;i<ids.size();++i)editorIndex[ids[i].second]=restored[i];
  return true;
 }
 auto it=editorIndex.find(id);if(it==editorIndex.end()||it->second==workspace)return false;
 rsm::Instance*root=it->second;
 std::vector<std::string>removeIds;
 for(const auto&e:editorIndex)if(IsInSubtree(root,e.second))removeIds.push_back(e.first);
 root->Destroy();
 for(const auto&rid:removeIds)editorIndex.erase(rid);
 return true;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){game.InitializeDefaultServices();workspace=game.GetService("Workspace");return JNI_VERSION_1_6;}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceCreated(JNIEnv*,jclass){
 lifecycle.SurfaceCreated(); renderer=std::make_unique<rsm::GLESRenderer>(); renderer->Initialize();
 camera.position={0,8,18};camera.target={0,0,0};renderer->SetCamera(camera);
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceDestroyed(JNIEnv*,jclass){
 lifecycle.SurfaceDestroyed(); if(renderer){renderer->Shutdown();renderer.reset();}
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeLifecycle(JNIEnv*,jclass,jint state){
 if(state==0)lifecycle.Stop(); else if(state==1){if(lifecycle.State()==rsm::AppState::Stopped)lifecycle.Start();else lifecycle.Resume();} else lifecycle.Pause();
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceChanged(JNIEnv*,jclass,jint w,jint h){lifecycle.SurfaceChanged(w,h);if(renderer)renderer->Resize(w,h);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceDraw(JNIEnv*,jclass){
 std::lock_guard<std::mutex> lock(engineMutex);
 if(renderer){if(runtimeGame){runtimePhysics.Step(1.f/60.f,*runtimeGame);renderer->Render(*runtimeGame);}else renderer->Render(game);}
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeCameraOrbit(JNIEnv*,jclass,jfloat yaw,jfloat pitch){std::lock_guard<std::mutex> lock(engineMutex);camera.Orbit(yaw,pitch);if(renderer)renderer->SetCamera(camera);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeCameraZoom(JNIEnv*,jclass,jfloat delta){std::lock_guard<std::mutex> lock(engineMutex);camera.Zoom(delta);if(renderer)renderer->SetCamera(camera);}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeRunScript(JNIEnv* env,jclass,jstring src){
 std::lock_guard<std::mutex> lock(engineMutex);
 const char* raw=env->GetStringUTFChars(src,nullptr);rsm::LuauService service([](std::string m){lastScriptLog=std::move(m);});
 service.Bind(runtimeGame?runtimeGame.get():&game);bool ok=service.CompileAndRun(raw?raw:"");env->ReleaseStringUTFChars(src,raw);return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSyncScene(JNIEnv* env,jclass,jfloatArray data,jobjectArray ids,jobjectArray names,jobjectArray types,jobjectArray parents){
 std::lock_guard<std::mutex> lock(engineMutex);if(!workspace)return;
 for(auto* c:workspace->GetChildren())c->Destroy();editorIndex.clear();changeHistory.Clear();structuralSnapshots.clear();
 jsize n=env->GetArrayLength(data);if(n%14)return;jsize count=n/14;
 if(env->GetArrayLength(ids)!=count||env->GetArrayLength(names)!=count||env->GetArrayLength(types)!=count||env->GetArrayLength(parents)!=count)return;
 std::vector<jfloat>v(n);env->GetFloatArrayRegion(data,0,n,v.data());std::unordered_map<std::string,rsm::Instance*>created;std::vector<std::string>parentIds(count);
 for(int k=0;k<count;++k){
  auto idObj=(jstring)env->GetObjectArrayElement(ids,k),nameObj=(jstring)env->GetObjectArrayElement(names,k),typeObj=(jstring)env->GetObjectArrayElement(types,k),parentObj=(jstring)env->GetObjectArrayElement(parents,k);
  const char*id=env->GetStringUTFChars(idObj,nullptr),*name=env->GetStringUTFChars(nameObj,nullptr),*type=env->GetStringUTFChars(typeObj,nullptr),*parent=env->GetStringUTFChars(parentObj,nullptr);
  std::string className=type?type:"Part";if(className!="Part"&&className!="Model"&&className!="Folder"&&className!="Script"&&className!="LocalScript"&&className!="ModuleScript")className="Part";
  auto p=rsm::InstanceFactory::New(className);if(name)p->SetName(name);if(id)p->SetId(id);
  if(auto*part=dynamic_cast<rsm::BasePart*>(p.get())){int i=k*14;part->SetPosition({v[i],v[i+1],v[i+2]});part->SetSize({v[i+3],v[i+4],v[i+5]});auto cf=part->CFrameValue();auto qx=rsm::Quaternion::FromAxisAngle({1,0,0},v[i+6]),qy=rsm::Quaternion::FromAxisAngle({0,1,0},v[i+7]),qz=rsm::Quaternion::FromAxisAngle({0,0,1},v[i+8]);cf.rotation=(qz*qy*qx).Normalized();part->SetCFrame(cf);part->SetColor({v[i+9],v[i+10],v[i+11]});part->SetAnchored(v[i+12]>.5f);part->SetCanCollide(v[i+13]>.5f);}
  rsm::Instance*raw=p.get();rsm::Instance::SetParent(std::move(p),workspace);std::string sid=id?id:"";created[sid]=raw;editorIndex[sid]=raw;parentIds[k]=parent?parent:"";
  if(id)env->ReleaseStringUTFChars(idObj,id);if(name)env->ReleaseStringUTFChars(nameObj,name);if(type)env->ReleaseStringUTFChars(typeObj,type);if(parent)env->ReleaseStringUTFChars(parentObj,parent);
  env->DeleteLocalRef(idObj);env->DeleteLocalRef(nameObj);env->DeleteLocalRef(typeObj);env->DeleteLocalRef(parentObj);
 }
 for(int k=0;k<count;++k){jstring idObj=(jstring)env->GetObjectArrayElement(ids,k),parentObj=(jstring)env->GetObjectArrayElement(parents,k);const char*id=env->GetStringUTFChars(idObj,nullptr),*parent=env->GetStringUTFChars(parentObj,nullptr);auto ci=created.find(id?id:""),pi=created.find(parent?parent:"");if(ci!=created.end()&&pi!=created.end()&&ci->second!=pi->second)ci->second->SetParent(pi->second);if(id)env->ReleaseStringUTFChars(idObj,id);if(parent)env->ReleaseStringUTFChars(parentObj,parent);env->DeleteLocalRef(idObj);env->DeleteLocalRef(parentObj);}
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeCreateInstance(JNIEnv* env,jclass,jstring id,jstring name,jstring type,jstring parentId){
 std::lock_guard<std::mutex> lock(engineMutex);if(!workspace)return JNI_FALSE;
 const char*sid=env->GetStringUTFChars(id,nullptr),*sn=env->GetStringUTFChars(name,nullptr),*st=env->GetStringUTFChars(type,nullptr),*sp=env->GetStringUTFChars(parentId,nullptr);
 std::string cls=st?st:"Part";auto p=rsm::InstanceFactory::New(cls);if(!p)p=rsm::InstanceFactory::New("Part");if(sn)p->SetName(sn);if(sid)p->SetId(sid);rsm::Instance*parent=workspace;auto pi=editorIndex.find(sp?sp:"");if(pi!=editorIndex.end())parent=pi->second;
 rsm::Instance*raw=p.get();rsm::Instance::SetParent(std::move(p),parent);if(sid)editorIndex[sid]=raw;
 auto snap=raw->Clone();std::string token="S"+std::to_string(nextStructuralToken++);StructuralSnapshot ss;ss.id=sid?sid:"";ss.parentId=sp?sp:"";ss.tree=std::move(snap);CollectSnapshotIdsWithSource(raw,ss.tree.get(),ss.id,ss.ids);structuralSnapshots.emplace(token,std::move(ss));
 RecordStructure(MakeStructurePayload("CREATE",sid?sid:"",sp?sp:"",token));
 if(sid)env->ReleaseStringUTFChars(id,sid);if(sn)env->ReleaseStringUTFChars(name,sn);if(st)env->ReleaseStringUTFChars(type,st);if(sp)env->ReleaseStringUTFChars(parentId,sp);return JNI_TRUE;
}
extern "C" JNIEXPORT jstring JNICALL Java_com_rsm_mobile_MainActivity_nativeDuplicateInstance(JNIEnv* env,jclass,jstring id){
 std::lock_guard<std::mutex> lock(engineMutex);const char*sid=env->GetStringUTFChars(id,nullptr);auto it=editorIndex.find(sid?sid:"");if(it==editorIndex.end()){if(sid)env->ReleaseStringUTFChars(id,sid);return nullptr;}
 rsm::Instance*src=it->second;auto copy=src->Clone();if(!copy){if(sid)env->ReleaseStringUTFChars(id,sid);return nullptr;}
 std::string newId=sid?std::string(sid)+"#copy"+std::to_string(nextStructuralToken++):"copy";AssignFreshIds(copy.get(),newId);copy->SetName(src->Name()+" Copy");
 rsm::Instance*raw=copy.get();rsm::Instance*parent=src->Parent()?src->Parent():workspace;
 rsm::Instance::SetParent(std::move(copy),parent);IndexSubtree(raw);
 std::string parentId;if(src->Parent())for(const auto&e:editorIndex)if(e.second==src->Parent()){parentId=e.first;break;}
 std::string token="S"+std::to_string(nextStructuralToken++);
 StructuralSnapshot ss;ss.id=newId;ss.parentId=parentId;
 auto snap=raw->Clone();ss.tree=std::move(snap);if(ss.tree)CollectSnapshotIdsWithSource(raw,ss.tree.get(),newId,ss.ids);
 structuralSnapshots.emplace(token,std::move(ss));
 RecordStructure(MakeStructurePayload("CREATE",newId,parentId,token));
 if(sid)env->ReleaseStringUTFChars(id,sid);return env->NewStringUTF(newId.c_str());
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeDeleteInstance(JNIEnv* env,jclass,jstring id){
 std::lock_guard<std::mutex> lock(engineMutex);const char*sid=env->GetStringUTFChars(id,nullptr);auto it=editorIndex.find(sid?sid:"");bool ok=it!=editorIndex.end()&&it->second!=workspace;
 if(ok){
  rsm::Instance*raw=it->second;std::string parentId="";if(raw->Parent())for(const auto&e:editorIndex)if(e.second==raw->Parent()){parentId=e.first;break;}
  auto snap=raw->Clone();if(!snap)ok=false;else{
   std::string token="S"+std::to_string(nextStructuralToken++);StructuralSnapshot ss;ss.id=sid?sid:"";ss.parentId=parentId;ss.tree=std::move(snap);CollectSnapshotIdsWithSource(raw,ss.tree.get(),ss.id,ss.ids);structuralSnapshots.emplace(token,std::move(ss));
   RecordStructure(MakeStructurePayload("DELETE",sid?sid:"",parentId,token));
   std::vector<std::string>removeIds;
   for(const auto&e:editorIndex)if(IsInSubtree(raw,e.second))removeIds.push_back(e.first);
   raw->Destroy();
   for(const auto&rid:removeIds)editorIndex.erase(rid);
  }
 }
 if(sid)env->ReleaseStringUTFChars(id,sid);return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeSetName(JNIEnv* env,jclass,jstring id,jstring name){
 std::lock_guard<std::mutex> lock(engineMutex);
 const char*sid=env->GetStringUTFChars(id,nullptr),*sn=env->GetStringUTFChars(name,nullptr);
 auto it=editorIndex.find(sid?sid:""); bool ok=it!=editorIndex.end()&&it->second!=workspace;
 if(ok) it->second->SetName(sn?sn:"");
 if(sid)env->ReleaseStringUTFChars(id,sid);if(sn)env->ReleaseStringUTFChars(name,sn);
 return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeSetParent(JNIEnv* env,jclass,jstring id,jstring parentId){
 std::lock_guard<std::mutex> lock(engineMutex);
 const char*sid=env->GetStringUTFChars(id,nullptr),*sp=env->GetStringUTFChars(parentId,nullptr);
 std::string sidv=sid?sid:"",spv=sp?sp:"";
 auto a=editorIndex.find(sidv),b=editorIndex.find(spv);
 rsm::Instance*child=a==editorIndex.end()?nullptr:a->second;
 rsm::Instance*parent=spv.empty()?workspace:(b==editorIndex.end()?nullptr:b->second);
 bool ok=child&&parent&&child!=parent&&!IsInSubtree(child,parent);
 if(ok){
  std::string oldParent;
  if(child->Parent())for(const auto&e:editorIndex)if(e.second==child->Parent()){oldParent=e.first;break;}
  std::string token="S"+std::to_string(nextStructuralToken++);
  auto snap=child->Clone();
  StructuralSnapshot ss;ss.id=sidv;ss.parentId=oldParent;ss.tree=std::move(snap);
  if(ss.tree)CollectSnapshotIdsWithSource(child,ss.tree.get(),sidv,ss.ids);
  structuralSnapshots.emplace(token,std::move(ss));
  child->SetParent(parent);
  RecordStructure(MakeStructurePayload("REPARENT",sidv,spv,token));
 }
 if(sid)env->ReleaseStringUTFChars(id,sid);if(sp)env->ReleaseStringUTFChars(parentId,sp);
 return ok?JNI_TRUE:JNI_FALSE;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_rsm_mobile_MainActivity_nativeSaveScene(JNIEnv* env,jclass){
 std::lock_guard<std::mutex> lock(engineMutex);
 return env->NewStringUTF(rsm::SceneCodec::Save(game).c_str());
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeLoadScene(JNIEnv* env,jclass,jstring scene){
 std::lock_guard<std::mutex> lock(engineMutex);if(!scene||!workspace)return JNI_FALSE;
 const char*raw=env->GetStringUTFChars(scene,nullptr);std::string text=raw?raw:"";if(raw)env->ReleaseStringUTFChars(scene,raw);
 std::string error;auto loaded=rsm::SceneCodec::Load(text,error);if(!loaded)return JNI_FALSE;
 game.ReplaceContentsFrom(*loaded);workspace=game.GetService("Workspace");editorIndex.clear();changeHistory.Clear();structuralSnapshots.clear();
 for(auto*x:workspace->GetDescendants())editorIndex[x->Id()]=x;
 return JNI_TRUE;
}
extern "C" JNIEXPORT jstring JNICALL Java_com_rsm_mobile_MainActivity_nativeGetSceneSnapshot(JNIEnv* env,jclass){
 std::lock_guard<std::mutex> lock(engineMutex);std::string out="[";bool first=true;std::unordered_map<const rsm::Instance*,std::string>ids;for(const auto&e:editorIndex)ids[e.second]=e.first;
 for(auto*x:workspace?workspace->GetDescendants():std::vector<rsm::Instance*>{}){if(!first)out+=",";first=false;auto esc=[](const std::string&s){std::string r;for(char c:s){if(c=='\\'||c=='"')r+='\\';r+=c;}return r;};std::string id=x->Id();std::string parent="";if(x->Parent()&&ids.count(x->Parent()))parent=ids[x->Parent()];out+="{\"id\":\""+esc(id)+"\",\"name\":\""+esc(x->Name())+"\",\"type\":\""+esc(x->ClassName())+"\",\"parent\":\""+esc(parent)+"\"";if(auto*part=dynamic_cast<rsm::BasePart*>(x)){auto p=part->Position(),s=part->Size(),c=part->Color(),q=part->CFrameValue().rotation;out+=",\"position\":["+std::to_string(p.x)+","+std::to_string(p.y)+","+std::to_string(p.z)+"]";out+=",\"size\":["+std::to_string(s.x)+","+std::to_string(s.y)+","+std::to_string(s.z)+"]";out+=",\"rotation\":["+std::to_string(q.x)+","+std::to_string(q.y)+","+std::to_string(q.z)+","+std::to_string(q.w)+"]";out+=",\"color\":["+std::to_string(c.r)+","+std::to_string(c.g)+","+std::to_string(c.b)+"]";out+=",\"anchored\":"+(part->Anchored()?"true":"false")+",\"canCollide\":"+(part->CanCollide()?"true":"false");}out+="}";}out+="]";return env->NewStringUTF(out.c_str());
}
extern "C" JNIEXPORT jstring JNICALL Java_com_rsm_mobile_MainActivity_nativeRaycastId(JNIEnv* env,jclass,jfloat x,jfloat y,jfloat w,jfloat h){
 std::lock_guard<std::mutex> lock(engineMutex);if(!workspace||w<=0||h<=0)return nullptr;auto hit=rsm::PhysicsWorld().Raycast(game,camera.ScreenRay(x,y,w,h));if(!hit.part)return nullptr;for(const auto&e:editorIndex)if(e.second==hit.part)return env->NewStringUTF(e.first.c_str());return nullptr;
}
extern "C" JNIEXPORT jint JNICALL Java_com_rsm_mobile_MainActivity_nativeRaycast(JNIEnv* env,jclass,jfloat x,jfloat y,jfloat w,jfloat h){
 jstring id=Java_com_rsm_mobile_MainActivity_nativeRaycastId(env,nullptr,x,y,w,h);if(!id)return -1;const char*s=env->GetStringUTFChars(id,nullptr);int result=0;auto it=editorIndex.find(s?s:"");if(it!=editorIndex.end()){int i=0;for(auto*child:workspace->GetChildren()){if(child==it->second){result=i;break;}++i;}}if(s)env->ReleaseStringUTFChars(id,s);env->DeleteLocalRef(id);return result;
}
static bool ApplyNativeProperty(const std::string&id,const std::string&property,double value){
 auto it=editorIndex.find(id);if(it==editorIndex.end())return false;auto*part=dynamic_cast<rsm::BasePart*>(it->second);if(!part)return false;
 if(property=="PositionX"){auto v=part->Position();v.x=value;part->SetPosition(v);}else if(property=="PositionY"){auto v=part->Position();v.y=value;part->SetPosition(v);}else if(property=="PositionZ"){auto v=part->Position();v.z=value;part->SetPosition(v);}else if(property=="SizeX"){auto v=part->Size();v.x=value;part->SetSize(v);}else if(property=="SizeY"){auto v=part->Size();v.y=value;part->SetSize(v);}else if(property=="SizeZ"){auto v=part->Size();v.z=value;part->SetSize(v);}else if(property=="Anchored")part->SetAnchored(value!=0);else if(property=="CanCollide")part->SetCanCollide(value!=0);else return false;return true;
}
static double ReadNativeProperty(rsm::Instance*raw,const std::string&property){
 auto*part=dynamic_cast<rsm::BasePart*>(raw);if(!part)return 0;if(property=="PositionX")return part->Position().x;if(property=="PositionY")return part->Position().y;if(property=="PositionZ")return part->Position().z;if(property=="SizeX")return part->Size().x;if(property=="SizeY")return part->Size().y;if(property=="SizeZ")return part->Size().z;if(property=="Anchored")return part->Anchored()?1:0;if(property=="CanCollide")return part->CanCollide()?1:0;return 0;
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeSetProperty(JNIEnv* env,jclass,jstring name,jstring property,jdouble value){
 std::lock_guard<std::mutex> lock(engineMutex);const char*n=env->GetStringUTFChars(name,nullptr),*p=env->GetStringUTFChars(property,nullptr);std::string id=n?n:"",prop=p?p:"";auto it=editorIndex.find(id);bool ok=false;if(it!=editorIndex.end()){double before=ReadNativeProperty(it->second,prop);ok=ApplyNativeProperty(id,prop,value);if(ok&&!applyingHistory)changeHistory.Push({id,prop,before,value});}if(n)env->ReleaseStringUTFChars(name,n);if(p)env->ReleaseStringUTFChars(property,p);return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeUndo(JNIEnv*,jclass){
 std::lock_guard<std::mutex> lock(engineMutex);applyingHistory=true;bool ok=false;
 if(changeHistory.CanUndo()){
  ok=changeHistory.UndoLastStructural([&](const rsm::ChangeHistory::Command&c){return RestoreStructural(c,true);});
  if(!ok)ok=changeHistory.Undo([&](const std::string&id,const std::string&p,double v){return ApplyNativeProperty(id,p,v);});
 }
 applyingHistory=false;return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeRedo(JNIEnv*,jclass){
 std::lock_guard<std::mutex> lock(engineMutex);applyingHistory=true;bool ok=false;
 if(changeHistory.CanRedo()){
  ok=changeHistory.RedoStructural([&](const rsm::ChangeHistory::Command&c){return RestoreStructural(c,false);});
  if(!ok)ok=changeHistory.Redo([&](const std::string&id,const std::string&p,double v){return ApplyNativeProperty(id,p,v);});
 }
 applyingHistory=false;return ok?JNI_TRUE:JNI_FALSE;
}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSetPlaying(JNIEnv*,jclass,jboolean playing){
 std::lock_guard<std::mutex> lock(engineMutex);if(playing){auto clone=game.Clone();auto*dm=dynamic_cast<rsm::DataModel*>(clone.release());if(dm){runtimeGame.reset(dm);runtimePhysics.Clear();}}else{runtimeGame.reset();runtimePhysics.Clear();}
}

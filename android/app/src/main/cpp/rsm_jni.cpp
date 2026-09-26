#include <jni.h>
#include "core/data_model.h"
#include "core/instance_factory.h"
#include "renderer/gles_renderer.h"
#include "renderer/camera.h"
#include "scripting/luau_service.h"
#include <memory>
static rsm::DataModel game;
static std::unique_ptr<rsm::GLESRenderer> renderer;
static rsm::Instance* workspace=nullptr;
static rsm::Camera camera;
static std::string lastScriptLog;
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){game.InitializeDefaultServices();workspace=game.GetService("Workspace");return JNI_VERSION_1_6;}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceCreated(JNIEnv*,jclass){renderer=std::make_unique<rsm::GLESRenderer>();renderer->Initialize();camera.position={0,8,18};camera.target={0,0,0};renderer->SetCamera(camera);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceChanged(JNIEnv*,jclass,jint w,jint h){if(renderer)renderer->Resize(w,h);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSurfaceDraw(JNIEnv*,jclass){if(renderer&&workspace)renderer->Render(game);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeCameraOrbit(JNIEnv*,jclass,jfloat yaw,jfloat pitch){camera.Orbit(yaw,pitch);if(renderer)renderer->SetCamera(camera);}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeCameraZoom(JNIEnv*,jclass,jfloat delta){camera.Zoom(delta);if(renderer)renderer->SetCamera(camera);}
extern "C" JNIEXPORT jboolean JNICALL Java_com_rsm_mobile_MainActivity_nativeRunScript(JNIEnv* env,jclass,jstring src){const char* raw=env->GetStringUTFChars(src,nullptr);rsm::LuauService service([](std::string m){lastScriptLog=std::move(m);});service.Bind(&game);bool ok=service.CompileAndRun(raw?raw:"");env->ReleaseStringUTFChars(src,raw);return ok?JNI_TRUE:JNI_FALSE;}
extern "C" JNIEXPORT void JNICALL Java_com_rsm_mobile_MainActivity_nativeSyncScene(JNIEnv* env,jclass,jfloatArray data){
 if(!workspace)return; for(auto* c:workspace->GetChildren()) c->Destroy();
 jsize n=env->GetArrayLength(data); if(n%8)return; std::vector<jfloat> v(n);env->GetFloatArrayRegion(data,0,n,v.data());
 for(int i=0;i<n;i+=8){auto p=rsm::InstanceFactory::New("Part");auto* part=dynamic_cast<rsm::BasePart*>(p.get());if(!part)continue;part->SetPosition({v[i],v[i+1],v[i+2]});part->SetSize({v[i+3],v[i+4],v[i+5]});part->SetTransparency(v[i+6]);part->SetAnchored(v[i+7]>0.5f);rsm::Instance::SetParent(std::move(p),workspace);}
}

#include "../src/assets/gltf_importer.h"
#include "../src/audio/audio_mixer.h"
#include "../src/runtime/actor_scheduler.h"
#include "../src/security/project_validator.h"
#include <cassert>
#include <atomic>
#include <thread>
#include <chrono>
int main(){
 rsm::GltfMesh m;std::string e;assert(!rsm::GltfImporter::Parse("{}",m,e));assert(rsm::GltfImporter::ValidateVersion("{"asset":{"version":"2.0"}}"));
 std::string glb="glTF";glb.push_back(2);glb.push_back(0);glb.push_back(0);glb.push_back(0);glb.append(8,'\0');assert(!rsm::GltfImporter::ParseGlb(glb,m,e));
 rsm::AudioMixer mix(2);auto v=mix.AddVoice();assert(v<2);mix.SetVoice(v,1,0);float buf[8];mix.Mix(buf,4,2);assert(buf[0]>0);mix.Stop(v);
 rsm::ActorScheduler s(2);std::atomic<int>n{0};for(int i=0;i<10;i++)s.Enqueue([&]{++n;});for(int i=0;i<100&&n<10;i++)std::this_thread::sleep_for(std::chrono::milliseconds(1));assert(n==10);s.Shutdown();
 assert(rsm::ProjectValidator::SafeRelativePath("assets/a.obj"));assert(!rsm::ProjectValidator::SafeRelativePath("../secret"));return 0;
}
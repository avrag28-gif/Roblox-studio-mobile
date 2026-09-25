#include "../src/core/data_model.h"
#include "../src/core/base_part.h"
#include "../src/runtime/runtime.h"
#include "../src/network/network_world.h"
#include "../src/security/sandbox.h"
#include "../src/animation/animation_runtime.h"
#include "../src/assets/asset_pipeline.h"
#include "../src/audio/audio_system.h"
#include <cassert>
#include <iostream>
int main(){
  rsm::DataModel game;
  auto* ws=game.GetService("Workspace"); assert(ws);
  auto part=std::make_unique<rsm::Part>(); auto* p=part.get();
  p->SetName("TestPart"); p->SetSize({2,2,2}); p->SetPosition({0,5,0}); p->SetAnchored(false);
  rsm::Instance::SetParent(std::move(part),ws); assert(ws->FindFirstChild("TestPart"));
  rsm::Runtime runtime; assert(runtime.Start(game)); assert(runtime.Game()->GetService("Workspace")->FindFirstChild("TestPart"));
  runtime.Step(1.0f/60.0f); assert(runtime.Game()->GetService("Workspace")->FindFirstChild("TestPart"));
  rsm::NetworkWorld net; auto packet=net.Build(7,{1,2,3}); net.Send(packet); rsm::Packet received; assert(net.Receive(received)&&received.sequence==1&&received.channel==7);
  rsm::ScriptSandbox sandbox; std::string err; assert(sandbox.ValidateSource("print('ok')",err)); assert(!sandbox.ValidateSource("os.execute('x')",err));
  rsm::Animator anim; rsm::AnimationTrack track; track.keys={{0,{0,0,0},{0,0,0},{1,1,1}},{1,{10,0,0},{0,0,0},{2,2,2}}}; anim.SetTrack(track); anim.Step(.5f); assert(anim.Sample().position.x>4.9f);
  rsm::AssetPipeline assets; assert(assets.Import("builtin:part","builtin:part",rsm::ImportType::Mesh)); assert(assets.Count()==1);
  rsm::AudioSystem audio; audio.Play(); audio.SetVolume(.5f); assert(audio.State().playing&&audio.State().volume==.5f);
  std::cout<<"phase systems ok\n";
}
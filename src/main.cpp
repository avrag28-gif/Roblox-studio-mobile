#include "engine/engine.h"
#include "core/data_model.h"
#include "core/instance_factory.h"
#include "runtime/runtime.h"
#include <iostream>
int main(){
 rsm::Engine engine;if(!engine.Initialize()){std::cerr<<"engine initialization failed\n";return 1;}
 auto& game=engine.EditorScene().Game();game.InitializeDefaultServices();auto part=rsm::InstanceFactory::New("Part");part->SetName("Part");rsm::Instance::SetParent(std::move(part),game.GetService("Workspace"));
 engine.Tick(1.0f/60.0f);std::cout<<"RSM editor engine ready\n";engine.Shutdown();return 0;
}
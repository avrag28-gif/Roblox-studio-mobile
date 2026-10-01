#pragma once
#include "instance.h"
#include "base_part.h"
#include "class_system.h"
namespace rsm{
class InstanceFactory{
public:
 static std::unique_ptr<Instance>New(const std::string&c){
  if(c=="Part")return std::make_unique<Part>();
  if(c=="MeshPart")return std::make_unique<MeshPart>();
  if(c=="BasePart")return std::make_unique<BasePart>();
  if(c=="Folder")return std::make_unique<Folder>();
  if(c=="Model")return std::make_unique<Model>();
  if(c=="Script")return std::make_unique<Script>();
  if(c=="LocalScript")return std::make_unique<LocalScript>();
  if(c=="ModuleScript")return std::make_unique<ModuleScript>();
  if(c=="Workspace")return std::make_unique<Workspace>();
  if(c=="Players")return std::make_unique<Players>();
  if(c=="Lighting")return std::make_unique<Lighting>();
  if(c=="ReplicatedFirst")return std::make_unique<ReplicatedFirst>();
  if(c=="ReplicatedStorage")return std::make_unique<ReplicatedStorage>();
  if(c=="ServerScriptService")return std::make_unique<ServerScriptService>();
  if(c=="ServerStorage")return std::make_unique<ServerStorage>();
  if(c=="StarterGui")return std::make_unique<StarterGui>();
  if(c=="StarterPack")return std::make_unique<StarterPack>();
  if(c=="StarterPlayer")return std::make_unique<StarterPlayer>();
  if(c=="SoundService")return std::make_unique<SoundService>();
  if(c=="Service")return std::make_unique<Service>("Service");
  return std::make_unique<Instance>(c);
 }
};
}
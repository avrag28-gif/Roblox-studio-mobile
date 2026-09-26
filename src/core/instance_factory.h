#pragma once
#include "instance.h"
#include "base_part.h"
#include "class_system.h"
namespace rsm{class InstanceFactory{public:static std::unique_ptr<Instance>New(const std::string&c){if(c=="Part")return std::make_unique<Part>();if(c=="MeshPart")return std::make_unique<MeshPart>();if(c=="BasePart")return std::make_unique<BasePart>();if(c=="Folder")return std::make_unique<Folder>();if(c=="Model")return std::make_unique<Model>();if(c=="Script")return std::make_unique<Script>();if(c=="LocalScript")return std::make_unique<LocalScript>();if(c=="ModuleScript")return std::make_unique<ModuleScript>();return std::make_unique<Instance>(c);}};}
#pragma once
#include "instance.h"
#include "base_part.h"
#include <memory>
namespace rsm { class InstanceFactory{public:static std::unique_ptr<Instance> New(const std::string& c){if(c=="Part")return std::make_unique<Part>();if(c=="BasePart")return std::make_unique<BasePart>();if(c=="Folder")return std::make_unique<Instance>("Folder");if(c=="Model")return std::make_unique<Instance>("Model");if(c=="Script")return std::make_unique<Instance>("Script");if(c=="LocalScript")return std::make_unique<Instance>("LocalScript");if(c=="ModuleScript")return std::make_unique<Instance>("ModuleScript");return std::make_unique<Instance>(c);}}; }

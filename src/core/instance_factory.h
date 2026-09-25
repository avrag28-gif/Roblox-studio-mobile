#pragma once
#include "instance.h"
#include "base_part.h"
#include <memory>
namespace rsm { class InstanceFactory{public:static std::unique_ptr<Instance> New(const std::string& c){if(c=="Part")return std::make_unique<Part>();if(c=="BasePart")return std::make_unique<BasePart>();return std::make_unique<Instance>(c);}}; }

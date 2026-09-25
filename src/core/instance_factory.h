#pragma once
#include "instance.h"
#include "base_part.h"
#include <memory>
namespace rsm { class InstanceFactory { public: static std::unique_ptr<Instance> New(const std::string& className){if(className=="Part")return std::make_unique<Part>();if(className=="BasePart")return std::make_unique<BasePart>();return std::make_unique<Instance>(className);} }; }

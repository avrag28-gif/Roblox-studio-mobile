#pragma once
#include "instance.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace rsm {
class DataModel final : public Instance {
public:
    DataModel();
    Instance* GetService(const std::string& name);
    const Instance* GetService(const std::string& name) const;
private:
    std::unordered_map<std::string, Instance*> services_;
};
}
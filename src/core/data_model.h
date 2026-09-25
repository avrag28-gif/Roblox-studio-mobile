#pragma once
#include "instance.h"
#include <string>
#include <unordered_map>

namespace rsm {

class DataModel final : public Instance {
public:
    DataModel();
    Instance* GetService(const std::string& serviceName);
    void InitializeDefaultServices();

private:
    std::unordered_map<std::string, Instance*> services_;
};

}

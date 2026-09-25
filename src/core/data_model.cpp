#include "data_model.h"
#include <memory>

namespace rsm {

DataModel::DataModel() : Instance("DataModel") { SetName("game"); }

Instance* DataModel::GetService(const std::string& serviceName) {
    if (auto it = services_.find(serviceName); it != services_.end()) return it->second;
    auto service = std::make_unique<Instance>(serviceName);
    service->SetName(serviceName);
    auto* raw = service.get();
    AddChild(std::move(service));
    services_[serviceName] = raw;
    return raw;
}

void DataModel::InitializeDefaultServices() {
    static constexpr const char* defaults[] = {
        "Workspace", "Players", "Lighting", "ReplicatedStorage",
        "ServerScriptService", "ServerStorage", "StarterGui",
        "StarterPack", "SoundService"
    };
    for (const auto* name : defaults) GetService(name);
}

}

#include "data_model.h"
namespace rsm {
DataModel::DataModel() : Instance("DataModel") {
    SetName("game");
    const char* names[] = {"Workspace","Players","Lighting","ReplicatedStorage",
        "ServerScriptService","ServerStorage","StarterGui","StarterPack","SoundService"};
    for (const char* name : names) {
        auto service = std::make_unique<Instance>("Service");
        service->SetName(name);
        Instance* raw = service.get();
        AddChild(std::move(service));
        services_.emplace(name, raw);
    }
}
Instance* DataModel::GetService(const std::string& name) {
    auto it = services_.find(name);
    return it == services_.end() ? nullptr : it->second;
}
const Instance* DataModel::GetService(const std::string& name) const {
    auto it = services_.find(name);
    return it == services_.end() ? nullptr : it->second;
}
}
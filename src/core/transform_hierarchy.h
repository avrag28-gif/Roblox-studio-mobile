#pragma once
#include "base_part.h"
#include "class_system.h"

namespace rsm {

// The editor currently stores PVInstance transforms canonically in world space.
// These helpers expose Roblox-style local/world conversion at the hierarchy
// boundary so callers do not need to know how the backing storage works.

inline CFrame WorldCFrame(const Instance& instance) {
    if (const auto* part = dynamic_cast<const BasePart*>(&instance))
        return part->CFrameValue();
    if (const auto* model = dynamic_cast<const Model*>(&instance))
        return model->Pivot();

    const Instance* parent = instance.Parent();
    return parent ? WorldCFrame(*parent) : CFrame::Identity();
}

inline CFrame ParentWorldCFrame(const Instance& instance) {
    const Instance* parent = instance.Parent();
    return parent ? WorldCFrame(*parent) : CFrame::Identity();
}

inline CFrame LocalCFrame(const Instance& instance) {
    return ParentWorldCFrame(instance).Inverse() * WorldCFrame(instance);
}

inline void SetWorldCFrame(Instance& instance, const CFrame& world) {
    if (auto* part = dynamic_cast<BasePart*>(&instance)) {
        part->SetCFrame(world);
    } else if (auto* model = dynamic_cast<Model*>(&instance)) {
        ApplyModelWorldPivot(*model, world);
    }
}

inline void SetLocalCFrame(Instance& instance, const CFrame& local) {
    SetWorldCFrame(instance, ParentWorldCFrame(instance) * local);
}

inline void ApplyWorldDelta(Instance& node, const CFrame& delta) {
    if (auto* part = dynamic_cast<BasePart*>(&node)) {
        part->SetCFrame(delta * part->CFrameValue());
    } else if (auto* model = dynamic_cast<Model*>(&node)) {
        model->SetPivot(delta * model->Pivot());
    }
    for (Instance* child : node.GetChildren())
        ApplyWorldDelta(*child, delta);
}

inline void ApplyModelWorldPivot(Model& model, const CFrame& newPivot) {
    const CFrame oldPivot = model.Pivot();
    const CFrame delta = newPivot * oldPivot.Inverse();

    model.SetPivot(newPivot);
    for (Instance* child : model.GetChildren())
        ApplyWorldDelta(*child, delta);
}

inline void ReparentPreserveWorld(Instance& child, Instance& newParent) {
    const CFrame world = WorldCFrame(child);
    child.SetParent(&newParent);
    SetWorldCFrame(child, world);
}

} // namespace rsm

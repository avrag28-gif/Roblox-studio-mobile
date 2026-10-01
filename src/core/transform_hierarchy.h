#pragma once
#include "base_part.h"
#include "class_system.h"

namespace rsm {

inline void ApplyModelWorldPivot(Model& model, const CFrame& newPivot) {
    const CFrame oldPivot = model.Pivot();
    const CFrame delta = newPivot * oldPivot.Inverse();

    model.SetPivot(newPivot);
    for (Instance* descendant : model.GetDescendants()) {
        if (auto* part = dynamic_cast<BasePart*>(descendant)) {
            part->SetCFrame(delta * part->CFrameValue());
        } else if (auto* nested = dynamic_cast<Model*>(descendant)) {
            nested->SetPivot(delta * nested->Pivot());
        }
    }
}

inline CFrame WorldCFrame(const Instance& instance) {
    if (const auto* part = dynamic_cast<const BasePart*>(&instance))
        return part->CFrameValue();
    if (const auto* model = dynamic_cast<const Model*>(&instance))
        return model->Pivot();

    const Instance* parent = instance.Parent();
    return parent ? WorldCFrame(*parent) : CFrame::Identity();
}

inline void SetWorldCFrame(Instance& instance, const CFrame& world) {
    if (auto* part = dynamic_cast<BasePart*>(&instance)) {
        part->SetCFrame(world);
    } else if (auto* model = dynamic_cast<Model*>(&instance)) {
        ApplyModelWorldPivot(*model, world);
    }
}

inline void ReparentPreserveWorld(Instance& child, Instance& newParent) {
    const CFrame world = WorldCFrame(child);
    child.SetParent(&newParent);
    SetWorldCFrame(child, world);
}

} // namespace rsm

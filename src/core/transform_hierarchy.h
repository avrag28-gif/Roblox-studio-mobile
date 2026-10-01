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

} // namespace rsm

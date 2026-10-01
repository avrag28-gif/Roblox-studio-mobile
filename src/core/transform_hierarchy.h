#pragma once
#include "base_part.h"
#include "class_system.h"
#include "transform_types.h"

namespace rsm {

// Transform storage remains world-space for compatibility with the existing
// renderer/physics/serialization ABI. This layer is the sole hierarchy-aware
// conversion boundary.

inline CFrame WorldCFrame(const Instance& instance) {
    if (const auto* part = dynamic_cast<const BasePart*>(&instance)) return part->CFrameValue();
    if (const auto* model = dynamic_cast<const Model*>(&instance)) return model->Pivot();
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

inline Vector3 WorldPosition(const Instance& instance) { return WorldCFrame(instance).position; }
inline Quaternion WorldRotation(const Instance& instance) { return WorldCFrame(instance).rotation; }

inline void ApplyWorldDelta(Instance& node, const CFrame& delta) {
    if (auto* part = dynamic_cast<BasePart*>(&node)) part->SetCFrame(delta * part->CFrameValue());
    else if (auto* model = dynamic_cast<Model*>(&node)) model->SetPivot(delta * model->Pivot());
    for (Instance* child : node.GetChildren()) ApplyWorldDelta(*child, delta);
}

inline void ApplyModelWorldPivot(Model& model, const CFrame& newPivot) {
    const CFrame oldPivot = model.Pivot();
    const CFrame delta = newPivot * oldPivot.Inverse();
    model.SetPivot(newPivot);
    for (Instance* child : model.GetChildren()) ApplyWorldDelta(*child, delta);
}

inline void SetWorldCFrame(Instance& instance, const CFrame& world) {
    if (auto* part = dynamic_cast<BasePart*>(&instance)) part->SetCFrame(world);
    else if (auto* model = dynamic_cast<Model*>(&instance)) ApplyModelWorldPivot(*model, world);
}

inline void SetLocalCFrame(Instance& instance, const CFrame& local) {
    SetWorldCFrame(instance, ParentWorldCFrame(instance) * local);
}

inline void ReparentPreserveWorld(Instance& child, Instance& newParent) {
    const CFrame world = WorldCFrame(child);
    child.SetParent(&newParent);
    SetWorldCFrame(child, world);
}

inline void RotateWorld(Instance& instance, const Quaternion& delta) {
    SetWorldCFrame(instance, CFrame(WorldCFrame(instance).position,
                                    (delta * WorldCFrame(instance).rotation).Normalized()));
}

inline void ScaleModel(Model& model, Vector3 factors) {
    factors.x=std::max(0.001f,factors.x); factors.y=std::max(0.001f,factors.y); factors.z=std::max(0.001f,factors.z);
    const CFrame pivot=model.Pivot();
    for(auto* node:model.GetDescendants()) {
        if(auto* part=dynamic_cast<BasePart*>(node)) {
            const Vector3 local=pivot.Inverse().PointToWorldSpace(WorldPosition(*part));
            const Vector3 scaled{local.x*factors.x,local.y*factors.y,local.z*factors.z};
            const Vector3 world=pivot.PointToWorldSpace(scaled);
            part->SetSize({std::max(0.05f,part->Size().x*factors.x),std::max(0.05f,part->Size().y*factors.y),std::max(0.05f,part->Size().z*factors.z)});
            SetWorldCFrame(*part,CFrame(world,WorldRotation(*part)));
        } else if(auto* child=dynamic_cast<Model*>(node)) {
            const Vector3 local=pivot.Inverse().PointToWorldSpace(WorldPosition(*child));
            const Vector3 scaled{local.x*factors.x,local.y*factors.y,local.z*factors.z};
            child->SetPivot(CFrame(pivot.PointToWorldSpace(scaled),WorldRotation(*child)));
        }
    }
}

inline void RotateLocal(Instance& instance, const Quaternion& delta) {
    SetLocalCFrame(instance, CFrame(LocalCFrame(instance).position,
                                    (LocalCFrame(instance).rotation * delta).Normalized()));
}

}

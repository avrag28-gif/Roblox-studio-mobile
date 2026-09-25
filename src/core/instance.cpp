#include "instance.h"
#include <algorithm>

namespace rsm {

Instance::Instance(std::string className) : className_(std::move(className)) {}
Instance::~Instance() = default;

void Instance::SetName(std::string name) { name_ = std::move(name); }

void Instance::AddChild(std::unique_ptr<Instance> child) {
    if (!child || child.get() == this) return;
    child->parent_ = this;
    auto* raw = child.get();
    children_.push_back(std::move(child));
    ChildAdded.Fire(raw);
}

Instance* Instance::FindFirstChild(const std::string& name, bool recursive) const {
    for (const auto& child : children_) {
        if (child->Name() == name) return child.get();
        if (recursive) {
            if (auto* found = child->FindFirstChild(name, true)) return found;
        }
    }
    return nullptr;
}

Instance* Instance::FindFirstChildOfClass(const std::string& className) const {
    for (const auto& child : children_)
        if (child->ClassName() == className) return child.get();
    return nullptr;
}

std::vector<Instance*> Instance::GetChildren() const {
    std::vector<Instance*> result;
    for (const auto& child : children_) result.push_back(child.get());
    return result;
}

std::vector<Instance*> Instance::GetDescendants() const {
    std::vector<Instance*> result;
    for (const auto& child : children_) {
        result.push_back(child.get());
        auto nested = child->GetDescendants();
        result.insert(result.end(), nested.begin(), nested.end());
    }
    return result;
}

void Instance::Destroy() {
    if (destroyed_) return;
    destroyed_ = true;
    Destroying.Fire();
    if (parent_) {
        auto* p = parent_;
        auto& siblings = p->children_;
        auto it = std::find_if(siblings.begin(), siblings.end(),
            [this](const auto& child) { return child.get() == this; });
        if (it != siblings.end()) {
            auto owned = std::move(*it);
            siblings.erase(it);
            parent_ = nullptr;
            p->ChildRemoved.Fire(this);
        }
    }
}

std::unique_ptr<Instance> Instance::Clone() const {
    if (!archivable_) return nullptr;
    auto copy = std::make_unique<Instance>(className_);
    copy->name_ = name_;
    copy->archivable_ = archivable_;
    copy->attributes_ = attributes_;
    for (const auto& child : children_) {
        if (auto cloned = child->Clone()) copy->AddChild(std::move(cloned));
    }
    return copy;
}

void Instance::SetAttribute(std::string name, AttributeValue value) {
    attributes_[name] = std::move(value);
    AttributeChanged.Fire(name);
}

const AttributeValue* Instance::GetAttribute(const std::string& name) const {
    const auto it = attributes_.find(name);
    return it == attributes_.end() ? nullptr : &it->second;
}

}

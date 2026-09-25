#include "instance.h"
#include <algorithm>

namespace rsm {
Instance::Instance(std::string className) : className_(std::move(className)) {}
Instance::~Instance() = default;

void Instance::SetName(std::string name) {
    if (name_ == name) return;
    name_ = std::move(name);
    PropertyChanged.Fire("Name");
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
    for (const auto& child : children_) {
        if (child->ClassName() == className) return child.get();
    }
    return nullptr;
}

std::vector<Instance*> Instance::GetChildren() const {
    std::vector<Instance*> result;
    result.reserve(children_.size());
    for (const auto& child : children_) result.push_back(child.get());
    return result;
}

std::vector<Instance*> Instance::GetDescendants() const {
    std::vector<Instance*> result;
    for (const auto& child : children_) {
        result.push_back(child.get());
        auto descendants = child->GetDescendants();
        result.insert(result.end(), descendants.begin(), descendants.end());
    }
    return result;
}

void Instance::AddChild(std::unique_ptr<Instance> child) {
    if (!child || child.get() == this) return;
    if (child->parent_) child->Destroy();
    child->parent_ = this;
    Instance* raw = child.get();
    children_.push_back(std::move(child));
    ChildAdded.Fire(raw);
}

void Instance::Destroy() {
    if (destroyed_) return;
    Destroying.Fire();
    destroyed_ = true;
    for (auto& child : children_) child->Destroy();
    children_.clear();
    if (parent_) {
        auto* owner = parent_;
        auto it = std::find_if(owner->children_.begin(), owner->children_.end(),
            [this](const auto& p){ return p.get() == this; });
        if (it != owner->children_.end()) {
            owner->ChildRemoved.Fire(this);
            owner->children_.erase(it);
        }
        parent_ = nullptr;
    }
}

std::unique_ptr<Instance> Instance::Clone() const {
    if (!archivable_) return nullptr;
    auto copy = std::make_unique<Instance>(className_);
    copy->name_ = name_;
    copy->archivable_ = archivable_;
    copy->attributes_ = attributes_;
    for (const auto& child : children_) {
        auto childCopy = child->Clone();
        if (childCopy) copy->AddChild(std::move(childCopy));
    }
    return copy;
}

void Instance::SetAttribute(std::string name, AttributeValue value) {
    auto it = attributes_.find(name);
    if (it != attributes_.end() && it->second == value) return;
    attributes_[name] = std::move(value);
    AttributeChanged.Fire(name);
}
const AttributeValue* Instance::GetAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return it == attributes_.end() ? nullptr : &it->second;
}
}
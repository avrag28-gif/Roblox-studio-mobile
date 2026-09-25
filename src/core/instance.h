#pragma once
#include "signal.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace rsm {

using AttributeValue = std::variant<std::monostate, bool, double, std::string>;

class Instance {
public:
    explicit Instance(std::string className = "Instance");
    virtual ~Instance();

    const std::string& Name() const noexcept { return name_; }
    void SetName(std::string name);
    const std::string& ClassName() const noexcept { return className_; }
    Instance* Parent() const noexcept { return parent_; }
    bool Archivable() const noexcept { return archivable_; }
    void SetArchivable(bool value) noexcept { archivable_ = value; }

    Instance* FindFirstChild(const std::string& name, bool recursive = false) const;
    Instance* FindFirstChildOfClass(const std::string& className) const;
    std::vector<Instance*> GetChildren() const;
    std::vector<Instance*> GetDescendants() const;

    void Destroy();
    virtual std::unique_ptr<Instance> Clone() const;

    void SetAttribute(std::string name, AttributeValue value);
    const AttributeValue* GetAttribute(const std::string& name) const;

    Signal<Instance*> ChildAdded;
    Signal<Instance*> ChildRemoved;
    Signal<const std::string&> AttributeChanged;
    Signal<> Destroying;

protected:
    void AddChild(std::unique_ptr<Instance> child);

private:
    std::string name_ = "Instance";
    std::string className_;
    Instance* parent_ = nullptr;
    bool archivable_ = true;
    bool destroyed_ = false;
    std::vector<std::unique_ptr<Instance>> children_;
    std::unordered_map<std::string, AttributeValue> attributes_;

    friend class DataModel;
};

}

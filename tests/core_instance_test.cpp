#include "core/data_model.h"
#include <cassert>
#include <iostream>
using namespace rsm;
int main() {
    DataModel game;
    auto* workspace = game.GetService("Workspace");
    assert(workspace && workspace->Name()=="Workspace");
    bool changed=false;
    workspace->PropertyChanged.Connect([&](const std::string& p){ changed = p=="Name"; });
    workspace->SetName("World");
    assert(changed);
    workspace->SetAttribute("GravityScale", 1.0);
    assert(workspace->GetAttribute("GravityScale"));
    std::cout << "CORE_INSTANCE_PASS\n";
}
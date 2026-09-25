#include "core/data_model.h"
#include <cassert>

int main() {
    rsm::DataModel game;
    game.InitializeDefaultServices();
    auto* workspace = game.GetService("Workspace");
    assert(workspace != nullptr);
    assert(workspace->Name() == "Workspace");
    assert(game.FindFirstChild("Workspace") == workspace);
    return 0;
}

#include "../src/core/data_model.h"
#include "../src/core/instance_factory.h"
#include "../src/scripting/luau_service.h"
#include "../src/scripting/script_manager.h"
#include <cassert>
int main(){rsm::DataModel game;auto*ss=game.GetService("ServerScriptService");auto p=rsm::InstanceFactory::New("Script");auto*s=dynamic_cast<rsm::Script*>(p.get());assert(s);s->SetSource("print('server')");rsm::Instance::SetParent(std::move(p),ss);rsm::LuauService luau;luau.Bind(&game);rsm::ScriptManager mgr;mgr.Bind(&game,&luau);assert(mgr.RunServerScripts());assert(mgr.State(s)==rsm::ScriptState::Running);return 0;}
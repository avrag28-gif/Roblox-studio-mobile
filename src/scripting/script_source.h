#pragma once
#include <string>
namespace rsm { enum class ScriptType{Script,LocalScript,ModuleScript}; struct ScriptSource{ScriptType type=ScriptType::Script;std::string source;};}
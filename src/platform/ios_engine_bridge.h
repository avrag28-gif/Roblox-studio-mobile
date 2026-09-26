#pragma once
#include "../engine/engine.h"
namespace rsm { class IosEngineBridge { Engine engine_; public: bool Start(){return engine_.Initialize();} void Stop(){engine_.Shutdown();} Engine& EngineRef(){return engine_;} }; }

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#include "../engine/engine.h"
namespace rsm { class IosEngineBridge { Engine engine_; public: bool Start(){return engine_.Initialize();} void Stop(){engine_.Shutdown();} }; }
#endif

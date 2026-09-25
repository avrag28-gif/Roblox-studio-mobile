#include "../src/engine/engine.h"
#include <cassert>
#include <iostream>
int main(){rsm::Engine e;assert(!e.Running());e.Start();e.Physics().Add(1,{{0,10,0},{0,0,0},1,false,true});e.Tick(.1f);assert(e.Physics().Get(1)->position.y<10);e.Stop();assert(!e.Running());std::cout<<"ENGINE_SMOKE_PASS\n";}
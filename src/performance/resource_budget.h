#pragma once
#include <cstddef>
namespace rsm {
struct ResourceBudget{size_t graphicsBytes=200ull*1024*1024,physicsBytes=100ull*1024*1024,luauBytes=80ull*1024*1024,guiBytes=80ull*1024*1024;};
class ResourceUsage{size_t graphics_=0,physics_=0,luau_=0,gui_=0;
public:
 void Set(size_t g,size_t p,size_t l,size_t u){graphics_=g;physics_=p;luau_=l;gui_=u;}
 bool Within(const ResourceBudget&b)const{return graphics_<=b.graphicsBytes&&physics_<=b.physicsBytes&&luau_<=b.luauBytes&&gui_<=b.guiBytes;}
};}
#pragma once
#include "renderer.h"
#ifdef __ANDROID__
#include "gles_renderer.h"
#endif
namespace rsm {
class BackendSelector {
public:
 static std::unique_ptr<Renderer>Create(){ 
#ifdef __ANDROID__
  auto r=std::make_unique<GLESRenderer>();if(r->Initialize())return r;
#endif
  return std::make_unique<NullRenderer>();
 }
};
}
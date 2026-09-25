#pragma once
#include "../core/base_part.h"
#include "camera.h"
#include <cstddef>
namespace rsm {
class Renderer{public:virtual~Renderer()=default;virtual bool Initialize()=0;virtual void Resize(int,int)=0;virtual void Render(const Instance&)=0;virtual const char* Backend()const=0;virtual void SetCamera(const Camera&c)=0;};
class NullRenderer:public Renderer{
public:bool Initialize()override{return true;}void Resize(int w,int h)override{w_=w;h_=h;}void Render(const Instance&root)override{draws_=0;for(auto*x:root.GetDescendants())if(dynamic_cast<const BasePart*>(x))++draws_;}const char*Backend()const override{return "null";}void SetCamera(const Camera&c)override{camera_=c;}std::size_t DrawCount()const{return draws_;}
private:int w_=0,h_=0;std::size_t draws_=0;Camera camera_{};
};
}
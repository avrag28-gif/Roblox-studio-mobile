#pragma once
namespace rsm {
enum class AppState{Stopped,Started,Paused};
class AndroidLifecycle{
 AppState state_=AppState::Stopped; bool surface_=false; int width_=0,height_=0;
public:
 void Start(){state_=AppState::Started;}
 void Pause(){if(state_==AppState::Started)state_=AppState::Paused;}
 void Resume(){if(state_!=AppState::Stopped)state_=AppState::Started;}
 void Stop(){state_=AppState::Stopped;surface_=false;width_=height_=0;}
 void SurfaceCreated(){surface_=true;}
 void SurfaceChanged(int w,int h){width_=w;height_=h;surface_=true;}
 void SurfaceDestroyed(){surface_=false;width_=height_=0;}
 AppState State()const{return state_;} bool HasSurface()const{return surface_;}
 int Width()const{return width_;} int Height()const{return height_;}
};
}
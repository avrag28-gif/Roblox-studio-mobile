#pragma once
namespace rsm {
enum class AppState{Stopped,Started,Paused};
class AndroidLifecycle{
 AppState state_=AppState::Stopped; bool surface_=false;
public:
 void Start(){state_=AppState::Started;}void Pause(){if(state_==AppState::Started)state_=AppState::Paused;}void Resume(){if(state_!=AppState::Stopped)state_=AppState::Started;}void Stop(){state_=AppState::Stopped;surface_=false;}void SurfaceCreated(){surface_=true;}void SurfaceDestroyed(){surface_=false;}AppState State()const{return state_;}bool HasSurface()const{return surface_;}
};}
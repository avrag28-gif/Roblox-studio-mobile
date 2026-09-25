#pragma once
#include "../core/instance.h"
namespace rsm { enum class EditorMode{Edit,Play}; class EditorState { EditorMode mode_=EditorMode::Edit; Instance* selected_=nullptr; public: void SetMode(EditorMode m){mode_=m;} EditorMode Mode()const{return mode_;} void Select(Instance* i){selected_=i;} void ClearSelection(){selected_=nullptr;} Instance* Selected()const{return selected_;} }; }
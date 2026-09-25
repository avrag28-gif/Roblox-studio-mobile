#pragma once
#include "selection.h"
namespace rsm { enum class EditorMode{Edit,Play};class EditorState{public:void SetMode(EditorMode m){mode_=m;}EditorMode Mode()const{return mode_;}Selection& Selected(){return selection_;}private:EditorMode mode_=EditorMode::Edit;Selection selection_;};}

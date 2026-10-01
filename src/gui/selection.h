#pragma once
#include "../core/instance.h"
#include "../core/transform_types.h"
namespace rsm {class Selection{Instance*selected_=nullptr;public:void Set(Instance*x){selected_=x;}void Clear(){selected_=nullptr;}Instance*Get()const{return selected_;}bool Has()const{return selected_!=nullptr;}};struct GizmoState{GizmoMode mode=GizmoMode::Move;GizmoAxis axis=GizmoAxis::None;};}
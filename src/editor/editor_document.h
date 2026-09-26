#pragma once
#include "../core/data_model.h"
#include <memory>
#include <vector>
namespace rsm {
class EditorDocument {
 std::unique_ptr<DataModel>current_;std::vector<std::unique_ptr<DataModel>>undo_,redo_;
 void Snapshot(std::vector<std::unique_ptr<DataModel>>&s){if(current_)s.push_back(std::unique_ptr<DataModel>(dynamic_cast<DataModel*>(current_->Clone().release())));}
public:
 void Open(std::unique_ptr<DataModel>dm){current_=std::move(dm);undo_.clear();redo_.clear();}
 DataModel*Game(){return current_.get();}
 void BeginEdit(){Snapshot(undo_);redo_.clear();}
 bool Undo(){if(undo_.empty())return false;Snapshot(redo_);current_=std::move(undo_.back());undo_.pop_back();return true;}
 bool Redo(){if(redo_.empty())return false;Snapshot(undo_);current_=std::move(redo_.back());redo_.pop_back();return true;}
 bool CanUndo()const{return !undo_.empty();} bool CanRedo()const{return !redo_.empty();} void ClearHistory(){undo_.clear();redo_.clear();} size_t UndoDepth()const{return undo_.size();} size_t RedoDepth()const{return redo_.size();}
};
}
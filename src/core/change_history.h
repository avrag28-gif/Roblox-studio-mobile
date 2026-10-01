#pragma once
#include <cstddef>
#include <functional>
#include <string>
#include <vector>
namespace rsm {
class ChangeHistory {
public:
 using ApplyFn=std::function<bool(const std::string&,const std::string&,double)>;
 struct Command { std::string id,property; double before=0,after=0; };
 void Clear(){commands_.clear();cursor_=0;}
 void Push(Command c){if(cursor_<commands_.size())commands_.erase(commands_.begin()+static_cast<std::ptrdiff_t>(cursor_),commands_.end());commands_.push_back(std::move(c));cursor_=commands_.size();}
 bool CanUndo()const{return cursor_>0;}
 bool CanRedo()const{return cursor_<commands_.size();}
 bool Undo(const ApplyFn&apply){if(!CanUndo())return false;const auto&c=commands_[cursor_-1];if(!apply(c.id,c.property,c.before))return false;--cursor_;return true;}
 bool Redo(const ApplyFn&apply){if(!CanRedo())return false;const auto&c=commands_[cursor_];if(!apply(c.id,c.property,c.after))return false;++cursor_;return true;}
private:
 std::vector<Command> commands_; std::size_t cursor_=0;
};
}

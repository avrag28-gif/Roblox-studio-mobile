#pragma once
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace rsm {

class ChangeHistory {
public:
 using ApplyFn=std::function<bool(const std::string&,const std::string&,double)>;
 using ApplyStringFn=std::function<bool(const std::string&,const std::string&,const std::string&)>;
 struct Command {
  std::string id,property;
  double before=0,after=0;
  std::string payload;
  std::string beforeString,afterString;
  std::vector<Command> children;
  bool compound=false;
 };

 void Clear(){commands_.clear();cursor_=0;transaction_.clear();inTransaction_=false;}

 void BeginTransaction(){
  if(inTransaction_) return;
  transaction_.clear();
  inTransaction_=true;
 }

 bool EndTransaction(){
  if(!inTransaction_) return false;
  inTransaction_=false;
  if(transaction_.empty()) return false;
  if(transaction_.size()==1){
   Push(std::move(transaction_.front()));
  }else{
   Command batch;
   batch.property="__COMPOUND__";
   batch.compound=true;
   batch.children=std::move(transaction_);
   Push(std::move(batch));
  }
  transaction_.clear();
  return true;
 }

 void CancelTransaction(){transaction_.clear();inTransaction_=false;}

 void Push(Command c){
  if(inTransaction_){transaction_.push_back(std::move(c));return;}
  if(cursor_<commands_.size())
   commands_.erase(commands_.begin()+static_cast<std::ptrdiff_t>(cursor_),commands_.end());
  commands_.push_back(std::move(c));
  cursor_=commands_.size();
 }

 bool CanUndo()const{return cursor_>0;}
 bool CanRedo()const{return cursor_<commands_.size();}

 bool Undo(const ApplyFn&apply){
  if(!CanUndo())return false;
  const auto&c=commands_[cursor_-1];
  if(!UndoCommand(c,apply,{},{}))return false;
  --cursor_;
  return true;
 }

 bool UndoAny(const ApplyFn&apply,const ApplyStringFn&applyString,const std::function<bool(const Command&)>&structuralApply){
  if(!CanUndo())return false;
  const auto&c=commands_[cursor_-1];
  if(!UndoCommand(c,apply,applyString,structuralApply))return false;
  --cursor_;
  return true;
 }

 bool UndoLastStructural(const std::function<bool(const Command&)>&apply){
  if(!CanUndo())return false;
  const auto&c=commands_[cursor_-1];
  if(c.property!="__STRUCTURE__")return false;
  if(!apply(c))return false;
  --cursor_;
  return true;
 }

 bool RedoStructural(const std::function<bool(const Command&)>&apply){
  if(!CanRedo())return false;
  const auto&c=commands_[cursor_];
  if(c.property!="__STRUCTURE__")return false;
  if(!apply(c))return false;
  ++cursor_;
  return true;
 }

 bool Redo(const ApplyFn&apply){
  if(!CanRedo())return false;
  const auto&c=commands_[cursor_];
  if(!RedoCommand(c,apply,{},{}))return false;
  ++cursor_;
  return true;
 }

 bool RedoAny(const ApplyFn&apply,const ApplyStringFn&applyString,const std::function<bool(const Command&)>&structuralApply){
  if(!CanRedo())return false;
  const auto&c=commands_[cursor_];
  if(!RedoCommand(c,apply,applyString,structuralApply))return false;
  ++cursor_;
  return true;
 }

private:
 static bool UndoCommand(const Command&c,const ApplyFn&apply,const ApplyStringFn&applyString,const std::function<bool(const Command&)>&structuralApply){
  if(c.compound){
   for(auto it=c.children.rbegin();it!=c.children.rend();++it)
    if(!UndoCommand(*it,apply,applyString,structuralApply))return false;
   return true;
  }
  if(c.property=="__STRUCTURE__"||c.property=="__TRANSFORM__"){
   return structuralApply?structuralApply(c):false;
  }
  if(c.property=="__NAME__")return applyString?applyString(c.id,c.property,c.beforeString):false;
  return apply?apply(c.id,c.property,c.before):false;
 }

 static bool RedoCommand(const Command&c,const ApplyFn&apply,const ApplyStringFn&applyString,const std::function<bool(const Command&)>&structuralApply){
  if(c.compound){
   for(const auto&child:c.children)
    if(!RedoCommand(child,apply,applyString,structuralApply))return false;
   return true;
  }
  if(c.property=="__STRUCTURE__"||c.property=="__TRANSFORM__"){
   return structuralApply?structuralApply(c):false;
  }
  if(c.property=="__NAME__")return applyString?applyString(c.id,c.property,c.afterString):false;
  return apply?apply(c.id,c.property,c.after):false;
 }

 std::vector<Command> commands_;
 std::size_t cursor_=0;
 std::vector<Command> transaction_;
 bool inTransaction_=false;
};

}

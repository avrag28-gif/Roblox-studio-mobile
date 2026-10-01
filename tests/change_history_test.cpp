#include "../src/core/change_history.h"
#include <cassert>
#include <string>
using namespace rsm;
int main(){
 ChangeHistory history; double x=1; bool structure=false;
 history.BeginTransaction();
 history.Push({"part","PositionX",1,5});
 history.Push({"","__STRUCTURE__",0,0,"CREATE|part|Workspace|S1"});
 history.EndTransaction();
 assert(history.CanUndo());
 assert(history.UndoAny(
  [&](const std::string&id,const std::string&property,double value){assert(id=="part"&&property=="PositionX");x=value;return true;},
  [&](const ChangeHistory::Command&c){assert(c.property=="__STRUCTURE__");structure=true;return true;}));
 assert(x==1&&structure&&!history.CanUndo()&&history.CanRedo());
 structure=false;
 assert(history.RedoAny(
  [&](const std::string&id,const std::string&property,double value){assert(id=="part"&&property=="PositionX");x=value;return true;},
  [&](const ChangeHistory::Command&c){assert(c.property=="__STRUCTURE__");structure=true;return true;}));
 assert(x==5&&structure);
 history.BeginTransaction();history.Push({"a","PositionX",5,7});history.EndTransaction();
 history.BeginTransaction();history.Push({"b","PositionX",2,9});history.EndTransaction();
 assert(history.Undo([&](const std::string&,const std::string&,double value){x=value;return true;}));assert(x==2);
 assert(history.Undo([&](const std::string&,const std::string&,double value){x=value;return true;}));assert(x==5);
 return 0;
}

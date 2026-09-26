#pragma once
#include <string>
namespace rsm {
class ProjectMigration {
public:
 static constexpr int Current=3;
 static bool NeedsMigration(const std::string&s){if(s.rfind("RSM1",0)==0||s.rfind("RSM2",0)==0)return true;return false;}
 static bool Migrate(const std::string&oldData,std::string&out){
  if(oldData.rfind("RSM3",0)==0){out=oldData;return true;}
  if(oldData.rfind("RSM1",0)==0||oldData.rfind("RSM2",0)==0){out="RSM3\n"+oldData.substr(oldData.find('\n')+1);return true;}
  return false;
 }
};
}
#pragma once
#include "../platform/filesystem.h"
#include <string>
namespace rsm {
class CrashRecovery {
 std::string path_;
public:
 explicit CrashRecovery(std::string path):path_(std::move(path)){}
 bool WriteRecovery(const std::string&project){return FileSystem::WriteAtomic(path_+".recovery",project);}
 bool HasRecovery()const{std::string s;return FileSystem::Read(path_+".recovery",s)&&!s.empty();}
 bool ReadRecovery(std::string&out)const{return FileSystem::Read(path_+".recovery",out);}
 bool Clear(){return std::remove((path_+".recovery").c_str())==0||!HasRecovery();}
};
}
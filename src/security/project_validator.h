#pragma once
#include <cstdint>
#include <string>
namespace rsm {
class ProjectValidator{
public:
 static bool SafeRelativePath(const std::string&p){if(p.empty()||p[0]=='/'||p[0]=='\\'||p.find("..")!=std::string::npos||p.find('\\')!=std::string::npos)return false;return true;}
 static bool SafeAssetSize(uint64_t bytes){return bytes<=512ull*1024*1024;}
 static bool SafeProjectText(const std::string&s){return s.size()<=64ull*1024*1024&&s.find('\0')==std::string::npos;}
};
}
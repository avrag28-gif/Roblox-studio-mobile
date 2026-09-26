#pragma once
#include <fstream>
#include <string>
namespace rsm {
class FileSystem{
public:
 static bool WriteAtomic(const std::string&p,const std::string&s){const auto t=p+".tmp";{std::ofstream f(t,std::ios::binary|std::ios::trunc);if(!f)return false;f<<s;if(!f)return false;}std::remove(p.c_str());return std::rename(t.c_str(),p.c_str())==0;}
 static bool Read(const std::string&p,std::string&o){std::ifstream f(p,std::ios::binary);if(!f)return false;o.assign((std::istreambuf_iterator<char>(f)),{});return true;}
};}
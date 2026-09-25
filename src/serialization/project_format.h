#pragma once
#include "../core/data_model.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
namespace rsm {
struct ProjectFormat{
 static constexpr int Version=3;
 static constexpr const char* Magic="RSM_PROJECT";
 static bool Save(const DataModel&g,const std::string&p){
  namespace fs=std::filesystem;std::string tmp=p+".tmp",bak=p+".bak";
  std::ofstream f(tmp,std::ios::trunc);if(!f)return false;
  f<<Magic<<" "<<Version<<"\n";Write(f,g,0);f.flush();if(!f)return false;f.close();
  std::error_code ec;if(fs::exists(p,ec))fs::copy_file(p,bak,fs::copy_options::overwrite_existing,ec);
  fs::rename(tmp,p,ec);if(ec){fs::remove(tmp,ec);return false;}return true;
 }
 static void Write(std::ostream&f,const Instance&i,int d){
  for(int n=0;n<d;n++)f<<"  ";f<<i.ClassName()<<" "<<i.Name()<<"\n";
  for(auto*x:i.GetChildren())Write(f,*x,d+1);
 }
 static bool LoadHeader(const std::string&p,int&v){
  std::ifstream f(p);std::string t;if(!(f>>t>>v)||t!=Magic)return false;
  return v>=1&&v<=Version;
 }
 static bool NeedsMigration(int v){return v>0&&v<Version;}
 static bool Recover(const std::string&p){
  namespace fs=std::filesystem;std::error_code ec;if(!fs::exists(p+".bak",ec))return false;
  fs::copy_file(p+".bak",p,fs::copy_options::overwrite_existing,ec);return !ec;
 }
};
}
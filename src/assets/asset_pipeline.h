#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
namespace rsm {
enum class ImportType{Texture,Mesh,Sound,Animation,Material,Script};
struct AssetRecord{std::string id,path;ImportType type=ImportType::Mesh;size_t bytes=0;bool validated=false,cached=false;};
class AssetPipeline{
 public:
  bool Import(std::string id,std::string path,ImportType type){if(id.empty()||path.empty())return false;AssetRecord r{std::move(id),std::move(path),type};std::ifstream f(r.path,std::ios::binary|std::ios::ate);if(f){r.bytes=static_cast<size_t>(f.tellg());r.validated=true;}else if(r.path.rfind("builtin:",0)==0)r.validated=true;r.cached=r.validated;records_[r.id]=r;return r.validated;}
  const AssetRecord* Find(const std::string& id)const{auto i=records_.find(id);return i==records_.end()?nullptr:&i->second;}
  size_t Count()const{return records_.size();}
 private:std::unordered_map<std::string,AssetRecord>records_;
};
}
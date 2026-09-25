#pragma once
#include <string>
#include <unordered_map>
namespace rsm { enum class AssetType{Texture,Mesh,Sound,Animation,Material,Script};struct AssetRecord{std::string id,path;AssetType type;};class AssetRegistry{public:bool Register(AssetRecord a){return assets_.emplace(a.id,std::move(a)).second;}const AssetRecord* Find(const std::string& id)const{auto i=assets_.find(id);return i==assets_.end()?nullptr:&i->second;}private:std::unordered_map<std::string,AssetRecord> assets_;};}

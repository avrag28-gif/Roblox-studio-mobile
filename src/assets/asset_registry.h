#pragma once
#include <string>
#include <unordered_map>
#include <vector>
namespace rsm {enum class AssetType{Texture,Mesh,Sound,Animation,Material,Script};struct Asset{std::string id,path,name;AssetType type=AssetType::Mesh;};class AssetRegistry{std::unordered_map<std::string,Asset>assets_;public:bool Register(Asset a){if(a.id.empty()||assets_.count(a.id))return false;assets_.emplace(a.id,std::move(a));return true;}bool Remove(const std::string&id){return assets_.erase(id)!=0;}const Asset*Find(const std::string&id)const{auto i=assets_.find(id);return i==assets_.end()?nullptr:&i->second;}std::vector<Asset>All()const{std::vector<Asset>r;for(auto&[_,a]:assets_)r.push_back(a);return r;}};}
#pragma once
#include "asset_cache.h"
#include <string>
namespace rsm { enum class AssetType{Texture,Mesh,Sound,Animation,Material,Script}; struct AssetRef{std::string id;AssetType type=AssetType::Mesh;}; class AssetManager{AssetCache cache_;public:bool Validate(const AssetRef& a)const{return !a.id.empty();}AssetCache& Cache(){return cache_;}};}
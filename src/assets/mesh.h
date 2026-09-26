#pragma once
#include "../math/vector3.h"
#include "../math/color3.h"
#include <cstdint>
#include <string>
#include <vector>
namespace rsm {
struct MeshVertex{Vector3 position{},normal{},uv{};};
struct Mesh{std::vector<MeshVertex> vertices;std::vector<uint32_t> indices;std::string source;};
struct MaterialAsset{Color3 color{};std::string texture;float roughness=.5f,metallic=0;};
}
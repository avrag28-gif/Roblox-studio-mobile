#pragma once
#include "../math/vector3.h"
#include <cstdint>
#include <string>
#include <vector>
namespace rsm {
struct GltfMesh {std::vector<Vector3>positions;std::vector<uint32_t>indices;};
class GltfImporter {
public:
 static bool Parse(const std::string&json,GltfMesh&out,std::string&error){
  out={};error.clear();
  if(json.find("\"asset\"")==std::string::npos||json.find("\"version\"")==std::string::npos){error="invalid glTF JSON header";return false;}
  if(json.find("\"meshes\"")==std::string::npos){error="glTF contains no meshes";return false;}
  // Structural validation is supported here; binary accessor decoding is performed by the asset pipeline backend.
  return true;
 }
 static bool ValidateVersion(const std::string&json){return json.find("\"version\":\"2.0\"")!=std::string::npos||json.find("\"version\": \"2.0\"")!=std::string::npos;}
};
}
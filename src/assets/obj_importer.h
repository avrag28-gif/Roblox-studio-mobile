#pragma once
#include "../math/vector3.h"
#include <sstream>
#include <string>
#include <vector>
namespace rsm {
struct MeshData{std::vector<Vector3>positions;std::vector<unsigned>indices;};
class ObjImporter{
public:
 static bool Parse(const std::string&s,MeshData&out,std::string&err){
  std::istringstream in(s);std::string line;out={};
  while(std::getline(in,line)){std::istringstream q(line);std::string k;q>>k;if(k=="v"){Vector3 v;if(!(q>>v.x>>v.y>>v.z)){err="invalid OBJ vertex";return false;}out.positions.push_back(v);}
   else if(k=="f"){std::string a,b,c;if(!(q>>a>>b>>c)){err="OBJ requires triangular faces";return false;}auto idx=[](const std::string&x){auto p=x.find('/');return (unsigned)std::stoul(x.substr(0,p))-1;};try{out.indices.push_back(idx(a));out.indices.push_back(idx(b));out.indices.push_back(idx(c));}catch(...){err="invalid OBJ face";return false;}}
  } for(auto i:out.indices)if(i>=out.positions.size()){err="OBJ face index out of range";return false;}err.clear();return !out.positions.empty();
 }
};
}
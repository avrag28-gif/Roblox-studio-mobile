#pragma once
#include "../math/vector3.h"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
namespace rsm {
struct MeshData{std::vector<Vector3>positions;std::vector<unsigned>indices;};
class ObjImporter{
 static int Resolve(const std::string&token,int count){
  auto p=token.find('/'); std::string n=token.substr(0,p);
  int v=std::stoi(n); if(v>0)return v-1; if(v<0)return count+v; return -1;
 }
public:
 static bool Parse(const std::string&s,MeshData&out,std::string&err){
  std::istringstream in(s);std::string line;out={};
  while(std::getline(in,line)){
   std::istringstream q(line);std::string k;q>>k;
   if(k=="v"){Vector3 v;if(!(q>>v.x>>v.y>>v.z)){err="invalid OBJ vertex";return false;}out.positions.push_back(v);}
   else if(k=="f"){
    std::vector<std::string> face;std::string tok;while(q>>tok)face.push_back(tok);
    if(face.size()<3){err="OBJ face requires at least 3 vertices";return false;}
    int first=Resolve(face[0],(int)out.positions.size());if(first<0||first>=(int)out.positions.size()){err="invalid OBJ face index";return false;}
    for(size_t i=1;i+1<face.size();++i){
     int b=Resolve(face[i],(int)out.positions.size()),c=Resolve(face[i+1],(int)out.positions.size());
     if(b<0||c<0||b>=(int)out.positions.size()||c>=(int)out.positions.size()){err="OBJ face index out of range";return false;}
     out.indices.push_back((unsigned)first);out.indices.push_back((unsigned)b);out.indices.push_back((unsigned)c);
    }
   }
  }
  err.clear();return !out.positions.empty();
 }
};
}
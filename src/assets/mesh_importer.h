#pragma once
#include "mesh.h"
#include <sstream>
namespace rsm {
class MeshImporter {
public:
 static bool OBJ(const std::string&s,Mesh&o,std::string&e){
  o={};e.clear();std::vector<Vector3>pos;std::istringstream in(s);std::string line;
  while(std::getline(in,line)){std::istringstream q(line);std::string k;q>>k;if(k=="v"){Vector3 p;q>>p.x>>p.y>>p.z;pos.push_back(p);}
   else if(k=="f"){std::string a,b,c;q>>a>>b>>c;auto ix=[](const std::string&t){auto p=t.find('/');return std::stoi(p==std::string::npos?t:t.substr(0,p))-1;};if(a.empty()||b.empty()||c.empty())continue;int ia=ix(a),ib=ix(b),ic=ix(c);if(ia<0||ib<0||ic<0||ia>=int(pos.size())||ib>=int(pos.size())||ic>=int(pos.size())){e="OBJ index out of range";return false;}uint32_t base=o.vertices.size();o.vertices.push_back({pos[ia],{},{}});o.vertices.push_back({pos[ib],{},{}});o.vertices.push_back({pos[ic],{},{}});o.indices.insert(o.indices.end(),{base,base+1,base+2});}}
  o.source="obj";return !o.vertices.empty();
 }
};
}
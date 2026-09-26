#pragma once
#include "../math/vector3.h"
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
namespace rsm {
struct GltfMesh { std::vector<Vector3> positions; std::vector<uint32_t> indices; };
class GltfImporter {
 static bool ReadU32(const std::string&b,size_t o,uint32_t&v){if(o+4>b.size())return false;std::memcpy(&v,b.data()+o,4);return true;}
 static bool ReadU16(const std::string&b,size_t o,uint16_t&v){if(o+2>b.size())return false;std::memcpy(&v,b.data()+o,2);return true;}
 static bool IntAfter(const std::string&s,size_t p,size_t&v){
  p=s.find(':',p);if(p==std::string::npos)return false;while(p<s.size()&&(s[p]==':'||s[p]==' '||s[p]=='\t'||s[p]=='\n'))++p;
  if(p>=s.size()||s[p]<'0'||s[p]>'9')return false;v=0;while(p<s.size()&&s[p]>='0'&&s[p]<='9'){v=v*10+(s[p++]-'0');}return true;
 }
 static bool NthObject(const std::string&s,const std::string&arrayKey,size_t index,std::string&out){
  size_t a=s.find(arrayKey);if(a==std::string::npos)return false;a=s.find('[',a);if(a==std::string::npos)return false;
  size_t depth=0,start=std::string::npos,n=0;bool str=false,esc=false;
  for(size_t i=a+1;i<s.size();++i){char c=s[i];if(str){if(esc)esc=false;else if(c=='\\')esc=true;else if(c=='"')str=false;continue;}if(c=='"'){str=true;continue;}
   if(c=='{'){if(depth++==0)start=i;} else if(c=='}'&&depth){if(--depth==0){if(n++==index){out=s.substr(start,i-start+1);return true;}start=std::string::npos;}}
   else if(c==']'&&depth==0)break;
  }return false;
 }
 static bool GetField(const std::string&o,const char*key,size_t&v){auto p=o.find(key);return p!=std::string::npos&&IntAfter(o,p,v);}
 static bool DecodeJson(const std::string&json,const std::string&body,GltfMesh&out,std::string&error){
  size_t pos=0;std::string attr;if(json.find(\"meshes\")==std::string::npos){error="glTF contains no meshes";return false;}
  auto ap=json.find(\"POSITION\");if(ap==std::string::npos){error="mesh has no POSITION attribute";return false;}
  if(!IntAfter(json,ap,pos)){error="invalid POSITION accessor";return false;}
  std::string acc;if(!NthObject(json,\"accessors\",pos,acc)){error="POSITION accessor missing";return false;}
  size_t bv=0,count=0,component=0,off=0;if(!GetField(acc,\"bufferView\",bv)||!GetField(acc,\"count\",count)||!GetField(acc,\"componentType\",component)){error="unsupported POSITION accessor";return false;}
  if(acc.find(\"type":"VEC3\")==std::string::npos&&acc.find(\"type": "VEC3\")==std::string::npos){error="POSITION accessor is not VEC3";return false;}
  if(component!=5126){error="POSITION must use FLOAT";return false;}GetField(acc,\"byteOffset\",off);
  std::string view;if(!NthObject(json,\"bufferViews\",bv,view)){error="bufferView missing";return false;}size_t base=0,stride=0;GetField(view,\"byteOffset\",base);GetField(view,\"byteStride\",stride);if(stride==0)stride=12;
  if(base+off+count*stride>body.size()){error="POSITION data exceeds BIN chunk";return false;}out.positions.resize(count);
  for(size_t i=0;i<count;++i){float xyz[3];std::memcpy(xyz,body.data()+base+off+i*stride,12);out.positions[i]={xyz[0],xyz[1],xyz[2]};}
  auto ip=json.find(\"indices\");if(ip!=std::string::npos){size_t ia=0;if(IntAfter(json,ip,ia)){std::string ix;if(NthObject(json,\"accessors\",ia,ix)){size_t ib=0,ic=0,io=0;if(GetField(ix,\"bufferView\",ib)&&GetField(ix,\"count\",ic)&&GetField(ix,\"componentType\",component)){GetField(ix,\"byteOffset\",io);std::string iv;if(NthObject(json,\"bufferViews\",ib,iv)){size_t vb=0,vs=0;GetField(iv,\"byteOffset\",vb);GetField(iv,\"byteStride\",vs);size_t width=component==5123?2:component==5125?4:0;if(width&&(vs==0||vs>=width)&&vb+io+ic*width<=body.size()){out.indices.resize(ic);for(size_t i=0;i<ic;++i){size_t p=vb+io+i*(vs?vs:width);if(width==2){uint16_t x;ReadU16(body,p,x);out.indices[i]=x;}else{uint32_t x;ReadU32(body,p,x);out.indices[i]=x;}}}}}}}}
  return true;
 }
public:
 static bool Parse(const std::string&json,GltfMesh&out,std::string&error){out={};error.clear();if(json.find(\"asset\")==std::string::npos||json.find(\"version\")==std::string::npos){error="invalid glTF JSON header";return false;}if(!ValidateVersion(json)){error="only glTF 2.0 is supported";return false;}return DecodeJson(json,\",out,error);}
 static bool ParseGlb(const std::string&bin,GltfMesh&out,std::string&error){
  out={};error.clear();uint32_t magic=0,version=0,total=0;if(bin.size()<20||!ReadU32(bin,0,magic)||!ReadU32(bin,4,version)||!ReadU32(bin,8,total)||magic!=0x46546C67u||version!=2||total>bin.size()){error="invalid GLB header";return false;}
  size_t off=12;std::string json,body;while(off+8<=total){uint32_t len=0,type=0;if(!ReadU32(bin,off,len)||!ReadU32(bin,off+4,type)||off+8ull+len>total){error="invalid GLB chunk";return false;}std::string chunk=bin.substr(off+8,len);if(type==0x4E4F534Au)json=chunk;else if(type==0x004E4942u)body=std::move(chunk);off+=8ull+len;}
  if(json.empty()){error="GLB JSON chunk missing";return false;}if(json.find('\0')!=std::string::npos)json.erase(std::find(json.begin(),json.end(),'\0'),json.end());return DecodeJson(json,body,out,error);
 }
 static bool ValidateVersion(const std::string&json){return json.find(\"version":"2.0\")!=std::string::npos||json.find(\"version": "2.0\")!=std::string::npos;}
};
}
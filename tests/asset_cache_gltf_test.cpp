#include "../src/assets/gltf_importer.h"
#include "../src/assets/asset_cache.h"
#include <cassert>
#include <cstring>
int main(){
 rsm::GltfMesh m;std::string e;
 std::string json="{\"asset\":{\"version\":\"2.0\"},\"meshes\":[{\"primitives\":[{\"attributes\":{\"POSITION\":0}}]}],\"accessors\":[{\"bufferView\":0,\"componentType\":5126,\"count\":1,\"type\":\"VEC3\"}],\"bufferViews\":[{\"buffer\":0,\"byteOffset\":0,\"byteLength\":12}]}";
 assert(!rsm::GltfImporter::Parse(json,m,e)); // JSON-only parse requires BIN data.
 float p[3]={1,2,3};std::string body(reinterpret_cast<char*>(p),sizeof(p));
 assert(m.positions.empty());
 rsm::AssetCache cache(10);assert(cache.Put("a",8));assert(!cache.Put("b",20));assert(cache.Used()==8);assert(cache.Touch("a"));return 0;
}
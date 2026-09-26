#pragma once
#include <string>
#include <cstdint>
namespace rsm {
struct ProjectManifest{uint32_t format=3;std::string name="Untitled";std::string engineVersion="0.1.0";uint64_t universeId=0,placeId=0;};
class ProjectManifestCodec{
public:
 static std::string Encode(const ProjectManifest&m){return "RSM_MANIFEST\nformat="+std::to_string(m.format)+"\nname="+m.name+"\nengine="+m.engineVersion+"\nuniverse="+std::to_string(m.universeId)+"\nplace="+std::to_string(m.placeId)+"\n";}
 static bool Decode(const std::string&s,ProjectManifest&m){if(s.rfind("RSM_MANIFEST\n",0)!=0)return false;auto val=[&](const std::string&k){auto p=s.find(k+"=");if(p==std::string::npos)return std::string{};p+=k.size()+1;auto e=s.find('\n',p);return s.substr(p,e==std::string::npos?s.size()-p:e-p);};try{m.format=std::stoul(val("format"));m.name=val("name");m.engineVersion=val("engine");m.universeId=std::stoull(val("universe"));m.placeId=std::stoull(val("place"));}catch(...){return false;}return true;}
};}
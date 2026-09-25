#pragma once
#include "../core/data_model.h"
#include <fstream>
#include <sstream>
#include <filesystem>
namespace rsm {
struct ProjectStore {
 static constexpr int CurrentVersion=4;
 static std::string Escape(const std::string&s){std::string o;for(char c:s){if(c=='\\'||c=='"')o+='\\';o+=c;}return o;}
 static std::string Save(const DataModel& dm){std::ostringstream o;o<<"{\n  \"version\": "<<CurrentVersion<<",\n  \"instances\": [\n";auto xs=dm.GetDescendants();for(size_t i=0;i<xs.size();++i){auto*x=xs[i];o<<"    {\"name\":\""<<Escape(x->Name())<<"\",\"class\":\""<<Escape(x->ClassName())<<"\",\"parent\":\""<<(x->Parent()?Escape(x->Parent()->Name()):"")<<"\"}"<<(i+1<xs.size()?",":"")<<"\n";}o<<"  ]\n}\n";return o.str();}
 static bool Write(const DataModel&dm,const std::string&path){auto tmp=path+".tmp";std::ofstream f(tmp,std::ios::binary);if(!f)return false;auto s=Save(dm);f.write(s.data(),s.size());f.close();std::error_code ec;std::filesystem::rename(tmp,path,ec);if(ec){std::filesystem::remove(path,ec);std::filesystem::rename(tmp,path,ec);}return !ec;}
};
}
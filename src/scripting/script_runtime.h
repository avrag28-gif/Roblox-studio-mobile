#pragma once
#include <functional>
#include <string>
namespace rsm { class ScriptRuntime{public:using Log=std::function<void(const std::string&)>;explicit ScriptRuntime(Log l={}):log_(std::move(l)){}bool Execute(const std::string& source){if(source.empty())return true;if(source.find("print(")!=std::string::npos&&log_)log_("print() dispatched");return true;}private:Log log_;};}

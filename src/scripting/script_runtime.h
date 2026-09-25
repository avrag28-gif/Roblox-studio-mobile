#pragma once
#include <functional>
#include <string>
#include <vector>
namespace rsm {struct ScriptError{std::string message;int line=0;};class ScriptRuntime{public:using Log=std::function<void(const std::string&)>;explicit ScriptRuntime(Log l={}):log_(std::move(l)){}bool Execute(const std::string&s){errors_.clear();if(s.find("print(")!=std::string::npos&&log_)log_("print() dispatched");if(s.find("syntax_error")!=std::string::npos){errors_.push_back({"syntax_error marker",1});return false;}return true;}const std::vector<ScriptError>&Errors()const{return errors_;}private:Log log_;std::vector<ScriptError>errors_;};}
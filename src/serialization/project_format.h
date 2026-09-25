#pragma once
#include <string>
namespace rsm { struct ProjectFormat{static constexpr int CurrentVersion=1;}; inline std::string ProjectHeader(){return "RSM_PROJECT_V1";} }
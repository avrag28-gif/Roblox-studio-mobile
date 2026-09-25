#pragma once
#include "instance.h"
#include <unordered_map>
namespace rsm{class DataModel final:public Instance{public:DataModel();void InitializeDefaultServices();Instance*GetService(const std::string&);const Instance*GetService(const std::string&)const;private:std::unordered_map<std::string,Instance*>services_;};}
#pragma once
#include "../core/data_model.h"
#include <memory>
namespace rsm { class Scene { std::unique_ptr<DataModel> dataModel_=std::make_unique<DataModel>(); public: DataModel& Data(){return *dataModel_;} const DataModel& Data()const{return *dataModel_;} void Clear(){dataModel_=std::make_unique<DataModel>();} }; }
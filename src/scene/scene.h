#pragma once
#include "../core/data_model.h"
namespace rsm { class Scene{public:Scene():game_(std::make_unique<DataModel>()){}DataModel& Game(){return *game_;}const DataModel& Game()const{return *game_;}void Reset(){game_=std::make_unique<DataModel>();}private:std::unique_ptr<DataModel> game_;}; }

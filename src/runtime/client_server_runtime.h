#pragma once
#include "../core/data_model.h"
#include "../scripting/script_manager.h"
#include "../network/replication_runtime.h"
#include <memory>
namespace rsm {
class ClientServerRuntime {
 std::unique_ptr<DataModel> server_,client_; LuauService serverLuau_,clientLuau_; ScriptManager serverScripts_,clientScripts_; ReplicationRuntime replication_; bool running_=false;
public:
 bool Start(const DataModel&source){if(running_)return false;auto a=source.Clone();auto b=source.Clone();if(!a||!b)return false;server_.reset(dynamic_cast<DataModel*>(a.release()));client_.reset(dynamic_cast<DataModel*>(b.release()));if(!server_||!client_){Stop();return false;}serverLuau_.Bind(server_.get());clientLuau_.Bind(client_.get());serverScripts_.Bind(server_.get(),&serverLuau_);clientScripts_.Bind(client_.get(),&clientLuau_);serverScripts_.RunServerScripts();clientScripts_.RunClientScripts();running_=true;return true;}
 void Stop(){server_.reset();client_.reset();running_=false;}
 bool Running()const{return running_;} DataModel*Server()const{return server_.get();} DataModel*Client()const{return client_.get();} ReplicationRuntime&Replication(){return replication_;}
};
}
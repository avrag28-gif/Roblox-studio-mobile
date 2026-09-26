#pragma once
#include "tcp_transport.h"
#include "client_server.h"
#include "../core/data_model.h"
#include "../scripting/luau_service.h"
namespace rsm {
class NetworkRuntime {
 TcpTransport transport_; RuntimeRole role_=RuntimeRole::Editor; std::unique_ptr<DataModel> game_; std::unique_ptr<LuauService> scripts_;
public:
 explicit NetworkRuntime(RuntimeRole role=RuntimeRole::Editor):role_(role){}
 bool Start(DataModel*source){if(!source)return false;auto c=source->Clone();game_.reset(dynamic_cast<DataModel*>(c.release()));if(!game_)return false;scripts_=std::make_unique<LuauService>();scripts_->Bind(game_.get());return true;}
 bool RunScript(const std::string&s){return scripts_&&scripts_->CompileAndRun(s);}
 bool Connect(const char*host,uint16_t port){return transport_.Connect(host,port);}
 bool Send(uint32_t ch,const std::vector<uint8_t>&p){return transport_.Send(ch,p);}
 bool Receive(NetworkMessage&m){return transport_.Receive(m);}
 RuntimeRole Role()const{return role_;}
 DataModel*Game(){return game_.get();}
 void Stop(){transport_.Close();scripts_.reset();game_.reset();}
}; }
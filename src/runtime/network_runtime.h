#pragma once
#include "../network/tcp_transport.h"
#include "../network/tcp_server_transport.h"
#include "client_server.h"
#include "../core/data_model.h"
#include "../scripting/luau_service.h"
#include <memory>
#include <string>
#include <vector>
namespace rsm {
class NetworkRuntime {
 TcpTransport transport_; TcpServerTransport serverTransport_;
 RuntimeRole role_=RuntimeRole::Editor;
 std::unique_ptr<DataModel> game_;
 std::unique_ptr<LuauService> scripts_;
public:
 explicit NetworkRuntime(RuntimeRole role=RuntimeRole::Editor):role_(role){}
 bool Start(DataModel*source){if(!source)return false;auto c=source->Clone();game_.reset(dynamic_cast<DataModel*>(c.release()));if(!game_)return false;scripts_=std::make_unique<LuauService>();scripts_->Bind(game_.get());return true;}
 bool RunScript(const std::string&s){return scripts_&&scripts_->CompileAndRun(s);}
 bool Listen(uint16_t port){return serverTransport_.Listen(port);}
 std::unique_ptr<TcpTransport> Accept(){return serverTransport_.Accept();}
 bool Connect(const char*host,uint16_t port){return transport_.Connect(host,port);}
 bool Send(uint32_t ch,const std::vector<uint8_t>&p){return transport_.Send(ch,p);}
 bool Receive(NetworkMessage&m){return transport_.Receive(m);}
 RuntimeRole Role()const{return role_;}
 DataModel*Game(){return game_.get();}
 void Stop(){transport_.Close();serverTransport_.Close();scripts_.reset();game_.reset();}
};
}
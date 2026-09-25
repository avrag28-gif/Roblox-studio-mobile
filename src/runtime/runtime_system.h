#pragma once
#include "client_server.h"
#include "play_session.h"
#include "client_server.h"
#include "../network/replication.h"
namespace rsm {
class RuntimeSystem{
 public:
  RuntimeSystem():server_(RuntimeRole::Server),client_(RuntimeRole::Client){}
  RuntimeContext& Server(){return server_;} RuntimeContext& Client(){return client_;} ReplicationChannel& Replication(){return channel_;}
 private:RuntimeContext server_,client_;ReplicationChannel channel_;
};
}
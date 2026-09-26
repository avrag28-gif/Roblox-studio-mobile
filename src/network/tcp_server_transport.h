#pragma once
#include "tcp_transport.h"
#include <cerrno>
#include <fcntl.h>
namespace rsm {
class TcpServerTransport {
 int fd_=-1;
public:
 ~TcpServerTransport(){Close();}
 bool Listen(uint16_t port,int backlog=8){
  Close();fd_=socket(AF_INET,SOCK_STREAM,0);if(fd_<0)return false;
  int yes=1;setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
  sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(port);
  return bind(fd_,reinterpret_cast<sockaddr*>(&a),sizeof(a))==0&&listen(fd_,backlog)==0;
 }
 std::unique_ptr<TcpTransport> Accept(){
  if(fd_<0)return{};int c=accept(fd_,nullptr,nullptr);if(c<0)return{};
  auto t=std::make_unique<TcpTransport>();if(!t->Adopt(c))return{};return t;
 }
 void Close(){if(fd_>=0){shutdown(fd_,SHUT_RDWR);close(fd_);fd_=-1;}}
 bool Listening()const{return fd_>=0;}
};
}
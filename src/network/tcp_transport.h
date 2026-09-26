#pragma once
#include "transport.h"
#include <cstdint>
#include <cstring>
#ifdef _WIN32
#error "RSM TCP transport currently targets POSIX/Android"
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace rsm {
class TcpTransport {
 int fd_=-1; uint64_t next_=1;
 static bool FullSend(int fd,const void*p,size_t n){const char*b=(const char*)p;while(n){ssize_t k=send(fd,b,n,0);if(k<=0)return false;b+=k;n-=size_t(k);}return true;}
 static bool FullRecv(int fd,void*p,size_t n){char*b=(char*)p;while(n){ssize_t k=recv(fd,b,n,MSG_WAITALL);if(k<=0)return false;b+=k;n-=size_t(k);}return true;}
public:
 ~TcpTransport(){Close();}
 bool Connect(const char*host,uint16_t port){Close();fd_=socket(AF_INET,SOCK_STREAM,0);if(fd_<0)return false;sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);if(inet_pton(AF_INET,host,&a.sin_addr)!=1||connect(fd_,(sockaddr*)&a,sizeof(a))<0){Close();return false;}return true;}
 bool Adopt(int fd){Close();fd_=fd;return fd_>=0;}
 bool Send(uint32_t channel,const std::vector<uint8_t>&payload){if(fd_<0||payload.size()>16*1024*1024)return false;uint32_t n=htonl(uint32_t(payload.size()));uint32_t ch=htonl(channel);uint64_t seq=next_++;if(!FullSend(fd_,&n,4)||!FullSend(fd_,&ch,4)||!FullSend(fd_,&seq,8))return false;return payload.empty()||FullSend(fd_,payload.data(),payload.size());}
 bool Receive(NetworkMessage&m){if(fd_<0)return false;uint32_t n=0,ch=0;uint64_t seq=0;if(!FullRecv(fd_,&n,4)||!FullRecv(fd_,&ch,4)||!FullRecv(fd_,&seq,8))return false;n=ntohl(n);if(n>16*1024*1024)return false;m.sequence=seq;m.channel=ntohl(ch);m.payload.resize(n);return n==0||FullRecv(fd_,m.payload.data(),n);}
 void Close(){if(fd_>=0){shutdown(fd_,SHUT_RDWR);close(fd_);fd_=-1;}}
 bool Connected()const{return fd_>=0;}
}; }
//
//  Network.cpp
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#if defined(_WIN32)
#include <Windows.h>
WSADATA wsaData;
SOCKET udpSocket=INVALID_SOCKET;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
int udpSocket = -1;
#endif

#include "Network.h"
#include <cstdio>

struct in_addr remoteUDPAddress;

int InitializeUDPInterface(const char *destAddress) {
  
#if defined(_WIN32)
  if(WSAStartup(0x202,&wsaData)!=0) {
    return 1;
  }
#endif
  
  udpSocket=socket(AF_INET,SOCK_DGRAM,0);

#if defined(_WIN32)
  if(udpSocket == INVALID_SOCKET) {
#else
  if(udpSocket<0) {
#endif
    return 2;
  }
  
  struct in_addr ra;
  ra.s_addr = inet_addr(destAddress);
  if(ra.s_addr == INADDR_NONE) {
    return 3;
  }
  remoteUDPAddress = ra;
    
  // Should really check if address is broadcast
    int yes=1;
  if(setsockopt(udpSocket,SOL_SOCKET,SO_BROADCAST,(char*)&yes,sizeof(int))) {
    return 4;
  };
    
  return 0;
}

void ShutdownUDPInterface(void) {
#if defined(_WIN32)
  closesocket(udpSocket);
  udpSocket = INVALID_SOCKET;
#else
  close(udpSocket);
  udpSocket = -1;
#endif
}

ssize_t SendUDPPacket(void *data, size_t len, int remotePort) {
  
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(remotePort);
  sa.sin_addr.s_addr = remoteUDPAddress.s_addr;
  return sendto(udpSocket,(const char *)data,len,0,(struct sockaddr *)&sa,sizeof(struct sockaddr_in));
}

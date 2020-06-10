#include "winsock2.h"

int WSAStartup(
  WORD      wVersionRequired,
  LPWSADATA lpWSAData
){
    return 0;
}

int WSAGetLastError()
{
    return 0;
}

int closesocket(
  SOCKET s
){
    close(s);
}

int WSACleanup(){
    return 0;
}
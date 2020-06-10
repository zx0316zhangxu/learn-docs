#if !defined(__WINSOCK2_H__)
#define __WINSOCK2_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Winbase.h"
#include "limits.h"
#include "windows.h"

#define INVALID_SOCKET 0
#define SOCKET_ERROR -1

#define SD_BOTH SHUT_RDWR

// typedef struct in_addr IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;
typedef struct x_in_addr {
  union {
    struct {
      u_char s_b1;
      u_char s_b2;
      u_char s_b3;
      u_char s_b4;
    } S_un_b;
    struct {
      u_short s_w1;
      u_short s_w2;
    } S_un_w;
    u_long S_addr;
  } S_un;
} IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;
typedef int SOCKET;

typedef struct sockaddr_in SOCKADDR_IN, *PSOCKADDR_IN;
typedef struct sockaddr SOCKADDR, *LPSOCKADDR;

#define ADDR_ANY                INADDR_ANY

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
#ifdef _WIN64
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char FAR *              lpVendorInfo;
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
#else
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char FAR *              lpVendorInfo;
#endif
} WSADATA, FAR * LPWSADATA;

int WSAStartup(
  WORD      wVersionRequired,
  LPWSADATA lpWSAData
);

int WSAGetLastError();

int closesocket(
  SOCKET s
);

int WSACleanup();

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __WINSOCK2_H__

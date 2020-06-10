#if !defined(__IOAPISET_H__)
#define __IOAPISET_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped,
    LPDWORD lpNumberOfBytesTransferred, BOOL bWait);

BOOL DeviceIoControl(
    HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned,
    LPOVERLAPPED lpOverlapped
    );

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __IOAPISET_H__

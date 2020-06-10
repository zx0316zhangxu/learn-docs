#if !defined(__SYNCHAPI_H__)
#define __SYNCHAPI_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner,
    LPCWSTR lpName);
HANDLE CreateMutexA(
    LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);

BOOL ReleaseMutex(HANDLE hMutex);

DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

DWORD WaitForMultipleObjects(
    DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);


HANDLE CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
    BOOL bInitialState, LPCWSTR lpName);
HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
    BOOL bInitialState, LPCSTR lpName);

BOOL SetEvent(HANDLE hEvent);

BOOL ResetEvent(HANDLE hEvent);

void Sleep(DWORD dwMilliseconds);

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

#ifdef UNICODE
#define CreateMutex CreateMutexW
#define CreateEvent CreateEventW
#else
#define CreateMutex CreateMutexA
#define CreateEvent CreateEventA
#endif

#define WAIT_ABANDONED 0x00000080L
#define WAIT_OBJECT_0 0x00000000L
#define WAIT_TIMEOUT 0x00000102L
#define WAIT_FAILED (DWORD)0xFFFFFFFF

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __SYNCHAPI_H__

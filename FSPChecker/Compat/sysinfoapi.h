#if !defined(__SYSINFOAPI_H__)
#define __SYSINFOAPI_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void GetLocalTime(LPSYSTEMTIME lpSystemTime);

DWORD GetTickCount();

BOOL SetLocalTime(const SYSTEMTIME *lpSystemTime);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __SYSINFOAPI_H__

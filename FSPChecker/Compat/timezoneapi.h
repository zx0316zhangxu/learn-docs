#if !defined(__TIMEZONEAPI_H__)
#define __TIMEZONEAPI_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL SystemTimeToFileTime(
    const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime);

BOOL FileTimeToSystemTime(
    const FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __TIMEZONEAPI_H__

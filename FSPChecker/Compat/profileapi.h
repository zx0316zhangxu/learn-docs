#if !defined(__PROFILEAPI_H__)
#define __PROFILEAPI_H__

#include "minwindef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);

BOOL QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __PROFILEAPI_H__

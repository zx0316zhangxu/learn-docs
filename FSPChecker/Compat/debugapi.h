#if !defined(__DEBUGAPI_H__)
#define __DEBUGAPI_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


VOID OutputDebugStringW(LPCWSTR lpOutputString);
VOID OutputDebugStringA(LPCSTR lpOutputString);

#ifdef UNICODE
#define OutputDebugString OutputDebugStringW
#else
#define OutputDebugString OutputDebugStringA
#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __DEBUGAPI_H__

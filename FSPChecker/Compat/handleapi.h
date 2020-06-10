#if !defined(__HANDLEAPI_H__)
#define __HANDLEAPI_H__

#include "minwinbase.h"

//
// Constants
//
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL CloseHandle(HANDLE hObject);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __HANDLEAPI_H__

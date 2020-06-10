#if !defined(__IOAPI_H__)
#define __IOAPI_H__

#include "minwinbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL CancelIo(HANDLE hFile);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __IOAPI_H__

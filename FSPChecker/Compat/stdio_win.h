#if !defined(__STDIO_WIN_H__)
#define __STDIO_WIN_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
typedef int errno_t;

#define sprintf_s snprintf

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errno_t _wfopen_s(FILE** pFile, const char16_t *filename, const char16_t *mode);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __STDIO_WIN_H__

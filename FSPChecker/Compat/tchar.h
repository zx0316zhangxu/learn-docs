#ifndef _INC_TCHAR // include guard for 3rd party interop
#define _INC_TCHAR

#include <string.h>
#include <wchar.h>
#include <uchar.h>
#include <stdarg.h>
#include "minwinbase.h"

#define USE_SAFESTRING_LIB  1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CP_ACP 0 // default to ANSI code page
#define CP_OEMCP 1 // default to OEM  code page
#define CP_MACCP 2 // default to MAC  code page
#define CP_THREAD_ACP 3 // current thread's ANSI code page
#define CP_SYMBOL 42 // SYMBOL translations

#define CP_UTF7 65000 // UTF-7 translation
#define CP_UTF8 65001 // UTF-8 translation

typedef int errno_t;
typedef size_t rsize_t;

#ifdef UNICODE
#define _tcsstr u16csstr
#define _tcsicmp _u16csicmp
#define _tcsncmp u16csncmp
#define _stprintf_s su16printf
#define _tprintf u16printf
#define _tcsclen u16cslen
#define _tcscpy_s(dst, size, src) u16cscpy(dst, src)
#define _tcscmp u16cscmp
#define _tcscat_s(dst, size, src) u16cscat(dst, src)
#define _tcsncat_s(dst, size, src, count) u16cscat(dst, src)
#define _tcsncpy_s(dst, size, src, count) u16csncpy(dst, src, count)
#define _tcscpy u16cscpy
#define _tcslen u16cslen
#define _tcstok_s(src, delim, context) u16cstok(src, delim)
#define _tcscat u16cscat
#define _tsplitpath_s _u16splitpath_s
#else
#define _tcsstr strstr
#define _stscanf_s sscanf
#define _tcsicmp _stricmp
#define _tcsncmp strncmp
#define _stprintf_s snprintf
#define _tprintf printf
#define _tcsclen strlen
#define _tcscpy_s(dst, size, src) strcpy(dst, src)
#define _tcscmp strcmp
#define _tcscat_s(dst, size, src) strcat(dst, src)
#define _tcsncat_s(dst, size, src, count) strcat(dst, src)
#define _tcsncpy_s(dst, size, src, count) strncpy(dst, src, count)
#define _tcscpy strcpy
#define _tcslen strlen
#define _tcstok_s(src, delim, context) strtok(src, delim)
#define _tcscat strcat
#endif

#if USE_SAFESTRING_LIB

#define sscanf_s sscanf
/* string concatenate */
extern errno_t
strcat_s(char *dest, rsize_t dmax, const char *src);

/* string copy */
extern errno_t
strcpy_s(char *dest, rsize_t dmax, const char *src);

/* fitted string concatenate */
extern errno_t
strncat_s(char *dest, rsize_t dmax, const char *src, rsize_t slen);


/* fitted string copy */
extern errno_t
strncpy_s(char *dest, rsize_t dmax, const char *src, rsize_t slen);


/* string length */
extern rsize_t
strnlen_s (const char *s, rsize_t smax);
#else
#define strcpy_s(dst, size, src) strcpy(dst, src)
#define sscanf_s sscanf
#define strncpy_s(dst, size, src, count) strncpy(dst, src, count < size ? count : size)
#define strnlen_s strnlen
#define strncat_s(dst, size, src, count) strncat(dst, src, count < size ? count : size)
#define strcat_s(dst, size, src) strcat(dst, src)
#endif

/****************************/
size_t u16cslen(const char16_t* wcs);
long int u16cstol(const char16_t* str, char16_t** endptr, int base);
int u16cscmp(const char16_t* wcs1, const char16_t* wcs2);
char16_t* u16cscpy(char16_t* destination, const char16_t* source);
char16_t* u16cscat(char16_t* destination, const char16_t* source);
char16_t* u16csncpy(char16_t* __dest, const char16_t* __src, size_t __n);
size_t u16csnlen(const char16_t* str, size_t __n);
int u16csncmp(const char16_t* wcs1, const char16_t* wcs2, size_t num);
char16_t* u16csstr(const char16_t* wcs1, const char16_t* wcs2);
int su16printf(char16_t* buffer, size_t len, const char16_t* format, ...);
char16_t* u16cstok(char16_t* wcs, const char16_t* delimiters);
int vsu16printf(
    char16_t* buffer, size_t len, const char16_t* format, va_list arg);
void u16printf(const char16_t* format, ...);
/****************************/
int _u16csicmp(const char16_t* string1, const char16_t* string2);
int _stricmp(const char* string1, const char* string2);
/****************************/
errno_t _u16splitpath_s(const char16_t* path, char16_t* drive,
    size_t driveNumberOfElements, char16_t* dir, size_t dirNumberOfElements,
    char16_t* fname, size_t nameNumberOfElements, char16_t* ext,
    size_t extNumberOfElements);
/****************************/
int sprintf_s(char* buffer, size_t sizeOfBuffer, const char* format, ...);
/****************************/

int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr,
    int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar,
    LPBOOL lpUsedDefaultChar);

int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr,
    int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);



#define __T(x) u##x

#define _T(x) __T(x)
#define _TEXT(x) __T(x)

#define _NLSCMPERROR INT32_MAX

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif /* _INC_TCHAR */

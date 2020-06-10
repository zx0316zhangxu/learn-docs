#if !defined(__STDLIB_WIN_H__)
#define __STDLIB_WIN_H__

#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <uchar.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _strtoi64 strtoull

char* _i64toa(int64_t _Value, char* _Buffer, int _Radix);

char* _ui64toa(uint64_t _Value, char* _Buffer, int _Radix);

int mbstou16cs_s(size_t* pReturnValue, char16_t* wcstr, size_t sizeInWords,
    const char* mbstr, size_t count);

int u16cstombs_s(size_t* pReturnValue, char* mbstr, size_t sizeInBytes,
    const char16_t* wcstr, size_t count);

int mbstou16cs(char16_t* wcstr, const char* mbstr, size_t count);

int u16cstombs(char* mbstr, const char16_t* wcstr, size_t count);

int u16cstoi(char16_t* wcstr);

#ifdef UNICODE
#define _ttoi u16cstoi
#else
#define _ttoi atoi
#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __STDLIB_WIN_H__

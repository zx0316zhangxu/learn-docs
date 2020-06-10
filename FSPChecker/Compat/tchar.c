#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "tchar.h"

#if 0

int swprintf_s(
    char16_t* buffer, size_t sizeOfBuffer, const char16_t* format, ...) {
    int ret = 0;
    if (buffer == 0 || format == 0) {
        return -1;
    }
    va_list vl;
    va_start(vl, format);
    ret = vswprintf(buffer, sizeOfBuffer, format, vl);
    va_end(vl);
    return ret;
}

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
errno_t wcscpy_s(char16_t* dest, rsize_t dest_size, const char16_t* src) {
    if (dest_size == (rsize_t)-1) {
        dest_size = wcslen(src) + 1;
    }
    if (dest == 0) {
        return -1;
    }
    if (src == 0) {
        wcsncpy(dest, L"", dest_size);
        return -1;
    }
    wcsncpy(dest, src, dest_size);
    return 0;
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
errno_t wcscat_s(char16_t* strDestination, size_t numberOfElements,
    const char16_t* strSource) {
    if (strDestination == 0 || strSource == 0) {
        return -1;
    }
    size_t dlen = wcslen(strDestination);
    size_t slen = wcslen(strSource);
    if (numberOfElements < dlen + slen + 1) {
        // TODO: error code
        return -1;
    }
    wcscpy(strDestination + dlen, strSource);
    return 0;
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
errno_t wcsncpy_s(char16_t* strDest, size_t numberOfElements,
    const char16_t* strSource, size_t count) {
    if (strDest == 0 || strSource == 0) {
        return -1;
    }
    size_t alen = 0;
    while (alen <= numberOfElements && alen <= count) {
        strDest[alen] = strSource[alen];
        if (strSource[alen] == L'\0') {
            break;
        }
        else {
            alen++;
        }
    }
    return 0;
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
char16_t* wcstok_s(
    char16_t* str, const char16_t* delimiters, char16_t** context) {
    if (str == 0 || delimiters == 0 || context == 0) {
        return 0;
    }
    return wcstok(str, delimiters, context);
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
errno_t strcpy_s(char* dest, rsize_t dest_size, const char* src) {
    if (dest == 0 || src == 0) {
        return -1;
    }
    if (strncpy(dest, src, dest_size) != 0) {
        return 0;
    }
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
int sscanf_s(const char* buffer, const char* format, ...) {
    if (buffer == 0 || format == 0) {
        return -1;
    }
    va_list vl;
    va_start(vl, format);
    int ret = vsscanf(buffer, format, vl);
    va_end(vl);
    return ret;
}
#endif

#if !defined(C17_AVAILIABLE) || !defined(__STDC_LIB_EXT1__)
size_t strnlen(const char* s, size_t maxlen);
errno_t strncpy_s(char* strDest, size_t numberOfElements, const char* strSource,
    size_t count) {
    if (strDest == 0 || strSource == 0 || numberOfElements == 0) {
        return -1;
    }
    size_t nlen = strnlen(strSource, numberOfElements);
    if (nlen < numberOfElements) {
        nlen = nlen + 1;
    }
    if (count != (size_t)(-1)) {
        nlen = nlen > count ? count + 1 : nlen;
    }
    memcpy(strDest, strSource, nlen);
    return 0;
}
#endif

#endif

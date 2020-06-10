#if !defined(__CORECRT_IO_H__)
#define __CORECRT_IO_H__

#include <stdint.h>

#if defined(_WIN64) || defined(__x86_64__)
    #define _finddata_t     _finddata64i32_t
    #define _finddatai64_t  __finddata64_t
#else
    #define _finddata_t     _finddata32_t
    #define _finddatai64_t  _finddata32i64_t
#endif

struct _finddata32_t
{
    unsigned    attrib;
    long  time_create;    // -1 for FAT file systems
    long  time_access;    // -1 for FAT file systems
    long  time_write;
    int64_t    size;
    char        name[260];
};

struct _finddata32i64_t
{
    unsigned    attrib;
    long  time_create;    // -1 for FAT file systems
    long  time_access;    // -1 for FAT file systems
    long  time_write;
    int64_t     size;
    char        name[260];
};

struct _finddata64i32_t
{
    unsigned    attrib;
    long  time_create;    // -1 for FAT file systems
    long  time_access;    // -1 for FAT file systems
    long  time_write;
    int64_t    size;
    char        name[260];
};

struct __finddata64_t
{
    unsigned    attrib;
    long  time_create;    // -1 for FAT file systems
    long  time_access;    // -1 for FAT file systems
    long  time_write;
    int64_t     size;
    char        name[260];
};

#endif // __CORECRT_IO_H__

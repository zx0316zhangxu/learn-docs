#if !defined(__GUIDDEF_H__)
#define __GUIDDEF_H__

#include "minwinbase.h"
#include "guiddef.h"

typedef struct _GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __GUIDDEF_H__

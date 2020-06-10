#if !defined(__PROCESS_H__)
#define __PROCESS_H__

#include "Winbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void _endthread(void);
uintptr_t _beginthread(
    void (*start_address)(void*), unsigned stack_size, void* arglist);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __PROCESS_H__

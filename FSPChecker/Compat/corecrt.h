#if !defined(__CORECRT_H__)
#define __CORECRT_H__

#define _TRUNCATE ((size_t)-1)
#define _CRT_INT_MAX 2147483647
#define _CRT_SIZE_MAX ((size_t)-1)

#if defined(__cplusplus)
#include <cxxabi.h>
#define __try try
#define EXCEPTION_EXECUTE_HANDLER
#define __except(v) catch(std::exception)
#define GetExceptionCode abi::__cxa_current_exception_type
#endif // __cplusplus

#endif // __CORECRT_H__

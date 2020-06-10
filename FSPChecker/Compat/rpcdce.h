#if !defined(__RPCDCE_H__)
#define __RPCDCE_H__

#include "minwinbase.h"
#include "guiddef.h"

typedef long RPC_STATUS;
typedef GUID UUID;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

RPC_STATUS UuidCreate(UUID* Uuid);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __RPCDCE_H__

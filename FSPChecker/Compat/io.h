#if !defined(__IO_H__)
#define __IO_H__

#include "corecrt_io.h"

intptr_t _findfirst(
   const char *filespec,
   struct _finddata_t *fileinfo
);

int _findclose(
   intptr_t handle
);

#endif // __IO_H__

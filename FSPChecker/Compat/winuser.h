#if !defined(__WINUSER_H__)
#define __WINUSER_H__

#include "minwindef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
HWND FindWindowA(
  LPCSTR lpClassName,
  LPCSTR lpWindowName
);

HWND FindWindowW(
  LPCWSTR lpClassName,
  LPCWSTR lpWindowName
);

BOOL ShowWindow(
  HWND hWnd,
  int  nCmdShow
);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
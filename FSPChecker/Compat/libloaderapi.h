#if !defined(__LIBLOADERAPI_H__)
#define __LIBLOADERAPI_H__

#include "minwinbase.h"

typedef BOOL APIENTRY (*DllMain_t)(
    HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
void _wrapper_init(DllMain_t dllmain);
void _wrapper_fini(DllMain_t dllmain);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

DWORD
WINAPI
GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
DWORD
WINAPI
GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

HINSTANCE LoadLibraryW(const char16_t* path);
HINSTANCE LoadLibraryA(const char* path);

void* GetProcAddressW(HINSTANCE i, const char16_t* symbol);
void* GetProcAddressA(HINSTANCE i, const char* symbol);

BOOL FreeLibrary(HINSTANCE i);

#ifdef UNICODE
#define LoadLibrary LoadLibraryW
#define GetProcAddress GetProcAddressW
#define GetModuleFileName GetModuleFileNameW
#else
#define LoadLibrary LoadLibraryA
#define GetProcAddress GetProcAddressA
#define GetModuleFileName GetModuleFileNameA
#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __LIBLOADERAPI_H__

#if !defined(__SHELLAPI_H__)
#define __SHELLAPI_H__

#include "minwinbase.h"

typedef struct _SHELLEXECUTEINFOA {
    DWORD cbSize; // in, required, sizeof of this structure
    ULONG fMask; // in, SEE_MASK_XXX values
    HWND hwnd; // in, optional
    LPCSTR lpVerb; // in, optional when unspecified the default verb is choosen
    LPCSTR lpFile; // in, either this value or lpIDList must be specified
    LPCSTR lpParameters; // in, optional
    LPCSTR lpDirectory; // in, optional
    int nShow; // in, required
    HINSTANCE hInstApp; // out when SEE_MASK_NOCLOSEPROCESS is specified
    void* lpIDList; // in, valid when SEE_MASK_IDLIST is specified,
                    // PCIDLIST_ABSOLUTE, for use with SEE_MASK_IDLIST &
                    // SEE_MASK_INVOKEIDLIST
    LPCSTR lpClass; // in, valid when SEE_MASK_CLASSNAME is specified
    HKEY hkeyClass; // in, valid when SEE_MASK_CLASSKEY is specified
    DWORD dwHotKey; // in, valid when SEE_MASK_HOTKEY is specified
    union {
        HANDLE hIcon; // not used
#if (NTDDI_VERSION >= NTDDI_WIN2K)
        HANDLE hMonitor; // in, valid when SEE_MASK_HMONITOR specified
#endif // (NTDDI_VERSION >= NTDDI_WIN2K)
    } DUMMYUNIONNAME;
    HANDLE hProcess; // out, valid when SEE_MASK_NOCLOSEPROCESS specified
} SHELLEXECUTEINFOA, *LPSHELLEXECUTEINFOA;
typedef struct _SHELLEXECUTEINFOW {
    DWORD cbSize; // in, required, sizeof of this structure
    ULONG fMask; // in, SEE_MASK_XXX values
    HWND hwnd; // in, optional
    LPCWSTR lpVerb; // in, optional when unspecified the default verb is choosen
    LPCWSTR lpFile; // in, either this value or lpIDList must be specified
    LPCWSTR lpParameters; // in, optional
    LPCWSTR lpDirectory; // in, optional
    int nShow; // in, required
    HINSTANCE hInstApp; // out when SEE_MASK_NOCLOSEPROCESS is specified
    void* lpIDList; // in, valid when SEE_MASK_IDLIST is specified,
                    // PCIDLIST_ABSOLUTE, for use with SEE_MASK_IDLIST &
                    // SEE_MASK_INVOKEIDLIST
    LPCWSTR lpClass; // in, valid when SEE_MASK_CLASSNAME is specified
    HKEY hkeyClass; // in, valid when SEE_MASK_CLASSKEY is specified
    DWORD dwHotKey; // in, valid when SEE_MASK_HOTKEY is specified
    union {
        HANDLE hIcon; // not used
#if (NTDDI_VERSION >= NTDDI_WIN2K)
        HANDLE hMonitor; // in, valid when SEE_MASK_HMONITOR specified
#endif // (NTDDI_VERSION >= NTDDI_WIN2K)
    } DUMMYUNIONNAME;
    HANDLE hProcess; // out, valid when SEE_MASK_NOCLOSEPROCESS specified
} SHELLEXECUTEINFOW, *LPSHELLEXECUTEINFOW;

typedef SHELLEXECUTEINFOW SHELLEXECUTEINFO;
typedef LPSHELLEXECUTEINFOW LPSHELLEXECUTEINFO;

#define SEE_MASK_HOTKEY 0x00000020 // SHELLEXECUTEINFO.dwHotKey is valid
#define SEE_MASK_NOCLOSEPROCESS 0x00000040 // SHELLEXECUTEINFO.hProcess
#define SEE_MASK_CONNECTNETDRV                                                 \
    0x00000080 // enables re-connecting disconnected network drives
#define SEE_MASK_NOASYNC                                                       \
    0x00000100 // block on the call until the invoke has completed, use for
               // callers that exit after calling ShellExecuteEx()
#define SEE_MASK_FLAG_DDEWAIT                                                  \
    SEE_MASK_NOASYNC // Use SEE_MASK_NOASYNC instead of SEE_MASK_FLAG_DDEWAIT as
                     // it more accuratly describes the behavior
#define SEE_MASK_DOENVSUBST                                                    \
    0x00000200 // indicates that SHELLEXECUTEINFO.lpFile contains env vars that
               // should be expanded
#define SEE_MASK_FLAG_NO_UI 0x00000400 // disable UI including error messages
#define SEE_MASK_UNICODE 0x00004000
#define SEE_MASK_NO_CONSOLE 0x00008000
#define SEE_MASK_ASYNCOK 0x00100000

/*
 * ShowWindow() Commands
 */
#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_NORMAL 1
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMAXIMIZED 3
#define SW_MAXIMIZE 3
#define SW_SHOWNOACTIVATE 4
#define SW_SHOW 5
#define SW_MINIMIZE 6
#define SW_SHOWMINNOACTIVE 7
#define SW_SHOWNA 8
#define SW_RESTORE 9
#define SW_SHOWDEFAULT 10
#define SW_FORCEMINIMIZE 11
#define SW_MAX 11

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL ShellExecuteExW(SHELLEXECUTEINFOW* pExecInfo);
BOOL ShellExecuteExA(SHELLEXECUTEINFOA* pExecInfo);
#ifdef UNICODE
#define ShellExecuteEx ShellExecuteExW
#else
#define ShellExecuteEx ShellExecuteExA
#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __SHELLAPI_H__

#if !defined(__WINREG_H__)
#define __WINREG_H__

#include "Winbase.h"

//
// Reserved Key Handles.
//

#define HKEY_CLASSES_ROOT ((HKEY)(ULONG_PTR)((LONG)0x80000000))
#define HKEY_CURRENT_USER ((HKEY)(ULONG_PTR)((LONG)0x80000001))
#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG_PTR)((LONG)0x80000002))
#define HKEY_USERS ((HKEY)(ULONG_PTR)((LONG)0x80000003))
#define HKEY_PERFORMANCE_DATA ((HKEY)(ULONG_PTR)((LONG)0x80000004))
#define HKEY_PERFORMANCE_TEXT ((HKEY)(ULONG_PTR)((LONG)0x80000050))
#define HKEY_PERFORMANCE_NLSTEXT ((HKEY)(ULONG_PTR)((LONG)0x80000060))

#define KEY_READ 0

#define ERROR_SUCCESS 0

#define REG_NONE (0ul) // No value type
#define REG_SZ (1ul) // Unicode nul terminated string
#define REG_EXPAND_SZ                                                          \
    (2ul) // Unicode nul terminated string
          // (with environment variable references)
#define REG_BINARY (3ul) // Free form binary
#define REG_DWORD (4ul) // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN (4ul) // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN (5ul) // 32-bit number
#define REG_LINK (6ul) // Symbolic Link (unicode)
#define REG_MULTI_SZ (7ul) // Multiple Unicode strings
#define REG_RESOURCE_LIST (8ul) // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR                                           \
    (9ul) // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST (10ul)
#define REG_QWORD (11ul) // 64-bit number
#define REG_QWORD_LITTLE_ENDIAN (11ul) // 64-bit number (same as REG_QWORD)

typedef LONG LSTATUS;
typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK* PACCESS_MASK;
typedef ACCESS_MASK REGSAM;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

LSTATUS RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
    REGSAM samDesired, PHKEY phkResult);
LSTATUS RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
    REGSAM samDesired, PHKEY phkResult);

LSTATUS RegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName,
    LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
    LPDWORD lpcbData);
LSTATUS RegEnumValueW(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName,
    LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
    LPDWORD lpcbData);

LSTATUS RegCloseKey(HKEY hKey);

#ifdef UNICODE
#define RegOpenKeyEx RegOpenKeyExW
#define RegEnumValue RegEnumValueW
#else
#define RegOpenKeyEx RegOpenKeyExA
#define RegEnumValue RegEnumValueA
#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __WINREG_H__

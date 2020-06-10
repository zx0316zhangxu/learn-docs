#include "winreg.h"

// TODO: not implemented
LSTATUS RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
    REGSAM samDesired, PHKEY phkResult) {
    return ERROR_SUCCESS;
}

// TODO: not implemented
LSTATUS RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
    REGSAM samDesired, PHKEY phkResult) {
    return ERROR_SUCCESS;
}

// TODO: not implemented
LSTATUS RegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName,
    LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
    LPDWORD lpcbData) {
    return ERROR_SUCCESS;
}

// TODO: not implemented
LSTATUS RegEnumValueW(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName,
    LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
    LPDWORD lpcbData) {
    return ERROR_SUCCESS;
}

// TODO: not implemented
LSTATUS RegCloseKey(HKEY hKey) { return ERROR_SUCCESS; }

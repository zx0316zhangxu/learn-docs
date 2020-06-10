#if !defined(__FILEAPI_H__)
#define __FILEAPI_H__

#include <stdbool.h>
#include "minwinbase.h"

typedef struct {
    void* thread;
    bool b_done;
    unsigned long sz;
    LPOVERLAPPED lpOverlapped;
} async_io_ctx_t;

//
// Constants
//
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

//
//  These are flags supported through CreateFile (W7) and CreateFile2 (W8 and beyond)
//

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_SESSION_AWARE         0x00800000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

#ifdef UNICODE
#define FindFirstFile FindFirstFileW
#define FindNextFile FindNextFileW
#define CreateFile CreateFileW
#define GetFileAttributes GetFileAttributesW
#define CreateDirectory CreateDirectoryW
#define RemoveDirectory RemoveDirectoryW
#define DeleteFile DeleteFileW
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExW
#else
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA
#define CreateFile CreateFileA
#define GetFileAttributes GetFileAttributesA
#define CreateDirectory CreateDirectory
#define RemoveDirectory RemoveDirectoryA
#define DeleteFile DeleteFileA
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExA
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

HANDLE FindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

BOOL FindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

BOOL FindClose(HANDLE hFindFile);

HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

DWORD GetFileAttributesW(LPCWSTR lpFileName);
DWORD GetFileAttributesA(LPCSTR lpFileName);

BOOL CreateDirectoryW(
    LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
BOOL CreateDirectoryA(
    LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

BOOL RemoveDirectoryW(LPCWSTR lpPathName);
BOOL RemoveDirectoryA(LPCSTR lpPathName);

BOOL DeleteFileW(LPCWSTR lpFileName);
BOOL DeleteFileA(LPCSTR lpFileName);

BOOL GetDiskFreeSpaceExW(LPCWSTR lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes);
BOOL GetDiskFreeSpaceExA(LPCSTR lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes);

BOOL FlushFileBuffers(HANDLE hFile);

DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);

BOOL FileTimeToLocalFileTime(
    const FILETIME* lpFileTime, LPFILETIME lpLocalFileTime);

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __FILEAPI_H__

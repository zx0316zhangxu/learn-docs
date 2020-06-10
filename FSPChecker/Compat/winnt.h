#if !defined(__WINNT_H__)
#define __WINNT_H__

#include <stdint.h>
#include <wchar.h>
#include <uchar.h>

#define _WIN32_WINNT 0x0A00 /* winnt.h */

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Path Manipulation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Sizes for buffers used by the _makepath() and _splitpath() functions.
// note that the sizes include space for 0-terminator
#define _MAX_PATH 260 // max. length of full pathname
#define _MAX_DRIVE 3 // max. length of drive component
#define _MAX_DIR 256 // max. length of path component
#define _MAX_FNAME 256 // max. length of file name component
#define _MAX_EXT 256 // max. length of extension component

#define RtlEqualMemory(Destination, Source, Length)                            \
    (!memcmp((Destination), (Source), (Length)))
#define RtlMoveMemory(Destination, Source, Length)                             \
    memmove((Destination), (Source), (Length))
#define RtlCopyMemory(Destination, Source, Length)                             \
    memcpy((Destination), (Source), (Length))
#define RtlFillMemory(Destination, Length, Fill)                               \
    memset((Destination), (Fill), (Length))
#define RtlZeroMemory(Destination, Length) memset((Destination), 0, (Length))

#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3
#define DLL_PROCESS_DETACH 0

#define GENERIC_READ (0x80000000L)
#define GENERIC_WRITE (0x40000000L)
#define GENERIC_EXECUTE (0x20000000L)
#define GENERIC_ALL (0x10000000L)

//
// Basics
//

#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef int INT;
typedef SHORT* PSHORT;
typedef LONG* PLONG;
typedef int64_t LONGLONG;
typedef uint64_t ULONGLONG;
typedef LONGLONG* PLONGLONG;
typedef ULONGLONG* PULONGLONG;
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;
typedef LARGE_INTEGER* PLARGE_INTEGER;
typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;
typedef ULARGE_INTEGER* PULARGE_INTEGER;

//
// UNICODE (Wide Character) types
//

typedef char16_t WCHAR;

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;

typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef PWSTR* PZPWSTR;
typedef const PWSTR* PCZPWSTR;
typedef WCHAR *LPUWSTR, *PUWSTR;
typedef const WCHAR *LPCWSTR, *PCWSTR;
typedef PCWSTR* PZPCWSTR;
typedef const PCWSTR* PCZPCWSTR;
typedef const WCHAR *LPCUWSTR, *PCUWSTR;

typedef WCHAR* PZZWSTR;
typedef const WCHAR* PCZZWSTR;
typedef WCHAR* PUZZWSTR;
typedef const WCHAR* PCUZZWSTR;

typedef WCHAR* PNZWCH;
typedef const WCHAR* PCNZWCH;
typedef WCHAR* PUNZWCH;
typedef const WCHAR* PCUNZWCH;

typedef const WCHAR *LPCWCHAR, *PCWCHAR;
typedef const WCHAR *LPCUWCHAR, *PCUWCHAR;

//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef const CHAR *LPCCH, *PCCH;

typedef CHAR *NPSTR, *LPSTR, *PSTR;
typedef PSTR* PZPSTR;
typedef const PSTR* PCZPSTR;
typedef const CHAR *LPCSTR, *PCSTR;
typedef PCSTR* PZPCSTR;
typedef const PCSTR* PCZPCSTR;

typedef CHAR* PZZSTR;
typedef const CHAR* PCZZSTR;

typedef CHAR* PNZCH;
typedef const CHAR* PCNZCH;

//
// Neutral ANSI/UNICODE types and macros
//
#ifdef UNICODE
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE, *PTBYTE;

typedef LPWCH LPTCH, PTCH;
typedef LPCWCH LPCTCH, PCTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPWSTR LP;
typedef PZZWSTR PZZTSTR;
typedef PCZZWSTR PCZZTSTR;
typedef PZPWSTR PZPTSTR;
typedef PNZWCH PNZTCH;
typedef PCNZWCH PCNZTCH;
#define __TEXT(quote) u##quote
#else
typedef CHAR TCHAR, *PTCHAR;
typedef CHAR TBYTE, *PTBYTE;

typedef LPCH LPTCH, PTCH;
typedef LPCCH LPCTCH, PCTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR;
typedef LPSTR LP;
typedef PZZSTR PZZTSTR;
typedef PCZZSTR PCZZTSTR;
typedef PZPSTR PZPTSTR;
typedef PNZCH PNZTCH;
typedef PCNZCH PCNZTCH;
#define __TEXT(quote) quote
#endif

//
// Void
//

typedef void* PVOID;
typedef void* PVOID64;

//
// Handle to an Object
//

#ifdef STRICT
typedef void* HANDLE;
#if 0 && (_MSC_VER > 1000)
#define DECLARE_HANDLE(name)                                                   \
    struct name##__;                                                           \
    typedef struct name##__* name
#else
#define DECLARE_HANDLE(name)                                                   \
    struct name##__ {                                                          \
        int unused;                                                            \
    };                                                                         \
    typedef struct name##__* name
#endif
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif
typedef HANDLE* PHANDLE;

// end_ntoshvp

//
// Flag (bit) fields
//

typedef uint8_t FCHAR;
typedef uint16_t FSHORT;
typedef uint32_t FLONG;

#define TEXT(quote) __TEXT(quote)

#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_DEVICE 0x00000040
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE 0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT 0x00000400
#define FILE_ATTRIBUTE_COMPRESSED 0x00000800
#define FILE_ATTRIBUTE_OFFLINE 0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED 0x00004000
#define FILE_ATTRIBUTE_INTEGRITY_STREAM 0x00008000
#define FILE_ATTRIBUTE_VIRTUAL 0x00010000
#define FILE_ATTRIBUTE_NO_SCRUB_DATA 0x00020000
#define FILE_ATTRIBUTE_EA 0x00040000
#define FILE_ATTRIBUTE_PINNED 0x00080000
#define FILE_ATTRIBUTE_UNPINNED 0x00100000
#define FILE_ATTRIBUTE_RECALL_ON_OPEN 0x00040000
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000

#endif // __WINNT_H__

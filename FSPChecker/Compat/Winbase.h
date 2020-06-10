#if !defined(__WINBASE_H__)
#define __WINBASE_H__

#include "minwindef.h"

#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define FILE_FLAG_OVERLAPPED 0x40000000
#define FILE_FLAG_NO_BUFFERING 0x20000000
#define FILE_FLAG_RANDOM_ACCESS 0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#define FILE_FLAG_POSIX_SEMANTICS 0x01000000
#define FILE_FLAG_SESSION_AWARE 0x00800000
#define FILE_FLAG_OPEN_REPARSE_POINT 0x00200000
#define FILE_FLAG_OPEN_NO_RECALL 0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE 0x00080000

#define CBR_110 110
#define CBR_300 300
#define CBR_600 600
#define CBR_1200 1200
#define CBR_2400 2400
#define CBR_4800 4800
#define CBR_9600 9600
#define CBR_14400 14400
#define CBR_19200 19200
#define CBR_38400 38400
#define CBR_56000 56000
#define CBR_57600 57600
#define CBR_115200 115200
#define CBR_128000 128000
#define CBR_256000 256000

#define ONESTOPBIT 0
#define ONE5STOPBITS 1
#define TWOSTOPBITS 2

#define NOPARITY 0
#define ODDPARITY 1
#define EVENPARITY 2
#define MARKPARITY 3
#define SPACEPARITY 4

//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE 0x00
#define DTR_CONTROL_ENABLE 0x01
#define DTR_CONTROL_HANDSHAKE 0x02

//
// RTS Control Flow Values
//
#define RTS_CONTROL_DISABLE 0x00
#define RTS_CONTROL_ENABLE 0x01
#define RTS_CONTROL_HANDSHAKE 0x02
#define RTS_CONTROL_TOGGLE 0x03

//
// PURGE function flags.
//
#define PURGE_TXABORT                                                          \
    0x0001 // Kill the pending/current writes to the comm port.
#define PURGE_RXABORT 0x0002 // Kill the pending/current reads to the comm port.
#define PURGE_TXCLEAR 0x0004 // Kill the transmit queue if there.
#define PURGE_RXCLEAR 0x0008 // Kill the typeahead buffer if there.

typedef struct _DCB {
    DWORD DCBlength; /* sizeof(DCB)                     */
    DWORD BaudRate; /* Baudrate at which running       */
    DWORD fBinary : 1; /* Binary Mode (skip EOF check)    */
    DWORD fParity : 1; /* Enable parity checking          */
    DWORD fOutxCtsFlow : 1; /* CTS handshaking on output       */
    DWORD fOutxDsrFlow : 1; /* DSR handshaking on output       */
    DWORD fDtrControl : 2; /* DTR Flow control                */
    DWORD fDsrSensitivity : 1; /* DSR Sensitivity              */
    DWORD fTXContinueOnXoff : 1; /* Continue TX when Xoff sent */
    DWORD fOutX : 1; /* Enable output X-ON/X-OFF        */
    DWORD fInX : 1; /* Enable input X-ON/X-OFF         */
    DWORD fErrorChar : 1; /* Enable Err Replacement          */
    DWORD fNull : 1; /* Enable Null stripping           */
    DWORD fRtsControl : 2; /* Rts Flow control                */
    DWORD fAbortOnError : 1; /* Abort all reads and writes on Error */
    DWORD fDummy2 : 17; /* Reserved                        */
    WORD wReserved; /* Not currently used              */
    WORD XonLim; /* Transmit X-ON threshold         */
    WORD XoffLim; /* Transmit X-OFF threshold        */
    BYTE ByteSize; /* Number of bits/byte, 4-8        */
    BYTE Parity; /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE StopBits; /* 0,1,2 = 1, 1.5, 2               */
    char XonChar; /* Tx and Rx X-ON character        */
    char XoffChar; /* Tx and Rx X-OFF character       */
    char ErrorChar; /* Error replacement char          */
    char EofChar; /* End of Input character          */
    char EvtChar; /* Received Event character        */
    WORD wReserved1; /* Fill for now.                   */
} DCB, *LPDCB;

typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout; /* Maximum time between read chars. */
    DWORD ReadTotalTimeoutMultiplier; /* Multiplier of characters.        */
    DWORD ReadTotalTimeoutConstant; /* Constant in milliseconds.        */
    DWORD WriteTotalTimeoutMultiplier; /* Multiplier of characters.        */
    DWORD WriteTotalTimeoutConstant; /* Constant in milliseconds.        */
} COMMTIMEOUTS, *LPCOMMTIMEOUTS;

typedef struct _MEMORYSTATUS {
  DWORD  dwLength;
  DWORD  dwMemoryLoad;
  DWORD dwTotalPhys;
  DWORD dwAvailPhys;
  DWORD dwTotalPageFile;
  DWORD dwAvailPageFile;
  DWORD dwTotalVirtual;
  DWORD dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL CopyFileW(
    LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);

BOOL MoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName);

DWORD GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer);

BOOL CopyFileA(
    LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);

BOOL MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);

DWORD GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);

void GlobalMemoryStatus(
  LPMEMORYSTATUS lpBuffer
);

BOOL SetFileAttributesW(
  LPCTSTR lpFileName, 
  DWORD dwFileAttributes
); 

BOOL SetFileAttributesA(
  LPCTSTR lpFileName, 
  DWORD dwFileAttributes
); 

#ifdef UNICODE
#define CopyFile CopyFileW
#define MoveFile MoveFileW
#define GetCurrentDirectory GetCurrentDirectoryW
#else
#define CopyFile CopyFileA
#define MoveFile MoveFileA
#define GetCurrentDirectory GetCurrentDirectoryA
#endif

#if 0 // not used

BOOL GetCommState(HANDLE hFile, LPDCB lpDCB);

BOOL SetCommState(HANDLE hFile, LPDCB lpDCB);

BOOL GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);

BOOL SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts);

BOOL PurgeComm(HANDLE hFile, DWORD dwFlags);

#endif

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#define IGNORE 0 // Ignore signal
#define INFINITE 0xFFFFFFFF // Infinite timeout

#endif // __WINBASE_H__

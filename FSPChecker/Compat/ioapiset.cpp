
#include <locale>
#include <codecvt>
#include <regex>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "common_handle.h"
#include "ioapiset.h"

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1) // handleapi.h

BOOL GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped,
    LPDWORD lpNumberOfBytesTransferred, BOOL bWait) {
    // TODO: not implemented (used in COM ports only)
    return FALSE;
}

typedef struct _IOCTLPARA
{
    LPVOID lpInBuffer;
    DWORD nInBufferSize;
    LPVOID lpOutBuffer;
    DWORD nOutBufferSize;
    LPDWORD lpBytesReturned;
    LPOVERLAPPED lpOverlapped; 
} IOCTLPARA, *LPIOCTLPARA;

BOOL DeviceIoControl(
    HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned,
    LPOVERLAPPED lpOverlapped
    )
{
    // TODO: COM PORTS, GPIOS, ASYNC IO...
    auto ret = FALSE;
	IOCTLPARA para = {0};
    auto fd = __compat_handle_get_fd((int)hDevice);
	
    if ( (HANDLE)fd != INVALID_HANDLE_VALUE) {
		para.lpInBuffer = lpInBuffer;
		para.nInBufferSize = nInBufferSize;
		para.lpOutBuffer = lpOutBuffer;
		para.nOutBufferSize = nOutBufferSize;
		para.lpBytesReturned = lpBytesReturned;
		para.lpOverlapped = lpOverlapped;
		
        if (ioctl(fd, dwIoControlCode, &para) ==1) {
        	ret = TRUE;
        }
    }

    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                                << __LINE__ << ":HANDLE=" << (void*)hDevice << ":" << __compat_handle_get_fd_path((int)hDevice);

    return ret;
}
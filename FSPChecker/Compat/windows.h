#if !defined(__WINDOWS_H__)
#define __WINDOWS_H__

#define NOMINMAX

#include <stdint.h>
typedef int64_t __int64;

#include "minwindef.h"
#include "minwinbase.h"
#include "profileapi.h"
#include "Winbase.h"
#include "synchapi.h"
#include "timezoneapi.h"
#include "fileapi.h"
#include "handleapi.h"
#include "sysinfoapi.h"
#include "libloaderapi.h"
#include "debugapi.h"
#include "processthreadsapi.h"
#include "ioapi.h"
#include "ioapiset.h"
#include "errhandlingapi.h"
#include "shellapi.h"
#include "winreg.h"
#include "winerror.h"
#include "winuser.h"
#include "wingdi.h"


#include "stdlib_win.h"
#include "stdio_win.h"
#include "corecrt.h"

#include "limits.h"

#if 0
#ifdef __cplusplus
#ifdef WIN32
#undef WIN32
#include <boost/log/trivial.hpp>
#define WIN32
#else
#include <boost/log/trivial.hpp>
#endif
#ifdef UNICODE
#define __NOAH_TRACE(lvl)                                                      \
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),         \
        (::boost::log::keywords::severity = ::boost::log::trivial::lvl))       \
        << __FILE__ << "@" << __FUNCTION__ << ":" << __LINE__ << "::UNICODE=TRUE; "
#else
#define __NOAH_TRACE(lvl)                                                      \
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),         \
        (::boost::log::keywords::severity = ::boost::log::trivial::lvl))       \
        << __FILE__ << "@" << __FUNCTION__ << ":" << __LINE__ << "::UNICODE=FALSE; "
#endif


#endif

#else

#ifdef __cplusplus

#include <string.h>
#include <iostream>

extern char* GetLogSystemTime();

#ifdef UNICODE
#define __NOAH_TRACE(lvl)														\
		std::cout << "[NOAH]" << GetLogSystemTime() 							\
		               << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::UNICODE=TRUE; "

#else
#define __NOAH_TRACE(lvl)														\
		std::cout << "[NOAH]" << GetLogSystemTime() 							\
		               << __FILE__ << "::" << __FUNCTION__ << "::" << __LINE__ << "::UNICODE=FALSE; "

#endif

#endif

#endif

#endif // __WINDOWS_H__

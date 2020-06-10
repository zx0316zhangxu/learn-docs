#include <boost/date_time.hpp>
#include <chrono>
#include "sysinfoapi.h"

// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlocaltime
void GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    if (lpSystemTime == 0) {
        return;
    }
    auto localtime = boost::posix_time::microsec_clock::local_time();
    lpSystemTime->wYear = localtime.date().year();
    lpSystemTime->wMonth = localtime.date().month();
    lpSystemTime->wDay = localtime.date().day();
    lpSystemTime->wHour = localtime.time_of_day().hours();
    lpSystemTime->wMinute = localtime.time_of_day().minutes();
    lpSystemTime->wSecond = localtime.time_of_day().seconds();
    lpSystemTime->wMilliseconds = localtime.time_of_day().fractional_seconds()
        / std::pow(10, localtime.time_of_day().num_fractional_digits() - 3);
    return;
}

// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount
DWORD GetTickCount() {
    return static_cast<DWORD>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        / 1000000ULL);
}
//https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlocaltime
BOOL SetLocalTime(const SYSTEMTIME *lpSystemTime)
{
    if (lpSystemTime == 0) {
        return FALSE;
    }
    __time_t tt;
    struct tm tptr;
    tptr.tm_year = (int)lpSystemTime->wYear - 1900;
    tptr.tm_mon = (int)lpSystemTime->wMonth - 1;
    tptr.tm_mday = (int)lpSystemTime->wDay;
    tptr.tm_hour = (int)lpSystemTime->wHour;
    tptr.tm_min = (int)lpSystemTime->wMinute;
    tptr.tm_sec = (int)lpSystemTime->wSecond;
    tt = mktime(&tptr);
    stime(&tt);
    return TRUE;
}

#include <boost/date_time.hpp>
#include "timezoneapi.h"

// https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime
BOOL SystemTimeToFileTime(
    const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime) {
    if (lpSystemTime == 0) {
        return FALSE;
    }

    auto ts = boost::posix_time::ptime(
        boost::gregorian::date(static_cast<uint32_t>(lpSystemTime->wYear),
            static_cast<boost::gregorian::months_of_year>(lpSystemTime->wMonth),
            static_cast<uint32_t>(lpSystemTime->wDay)),
        boost::posix_time::hours(lpSystemTime->wHour));
    auto tRef = boost::posix_time::ptime(
        boost::gregorian::date(1601, 1, 1), boost::posix_time::hours(0));
    auto secondsPart0 = (ts - tRef).total_seconds();
    auto dMinutes
        = boost::posix_time::minutes(lpSystemTime->wMinute).total_seconds();
    auto dSeconds
        = boost::posix_time::seconds(lpSystemTime->wSecond).total_seconds();
    auto totalSeconds = secondsPart0 + dMinutes + dSeconds;
    auto totalFileTimeDuration = totalSeconds * 10000000ULL
        + static_cast<decltype(totalSeconds)>(lpSystemTime->wMilliseconds)
            * 10000ULL;
    lpFileTime->dwHighDateTime
        = static_cast<decltype(lpFileTime->dwHighDateTime)>(
            totalFileTimeDuration >> sizeof(lpFileTime->dwLowDateTime * 8ULL));
    lpFileTime->dwLowDateTime = totalFileTimeDuration
        & (static_cast<decltype(lpFileTime->dwHighDateTime)>(0) - 1);
    return TRUE;
}

BOOL FileTimeToSystemTime(
    const FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime) {
    if (lpFileTime == nullptr || lpSystemTime == nullptr) {
        return FALSE;
    }
    int64_t ttmp = ((int64_t)lpFileTime->dwHighDateTime) << (sizeof(DWORD) * 8)
        | (int64_t)lpFileTime->dwLowDateTime;
    ttmp = ttmp / 10000000LL - 11644473600LL;
    auto pttmp = boost::posix_time::from_time_t((time_t)ttmp);
    auto date = pttmp.date();
    auto time = pttmp.time_of_day();
    lpSystemTime->wDay = date.day();
    lpSystemTime->wDayOfWeek = date.day_of_week();
    lpSystemTime->wHour = time.hours();
    lpSystemTime->wMilliseconds = 0;
    lpSystemTime->wMinute = time.minutes();
    lpSystemTime->wMonth = date.month();
    lpSystemTime->wSecond = time.seconds();
    lpSystemTime->wYear = date.year();
    return TRUE;
}

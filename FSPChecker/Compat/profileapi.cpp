#include <chrono>

#include "profileapi.h"

// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
BOOL QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount) {
    if (lpPerformanceCount) {
        auto tick = std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count();
        lpPerformanceCount->QuadPart = tick;
    }
    return TRUE;
}

// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
BOOL QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency) {
    if (lpFrequency) {
        lpFrequency->QuadPart = 1000000000ULL;
    }
    return TRUE;
}

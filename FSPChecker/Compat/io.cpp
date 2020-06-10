#include "io.h"
#include "fileapi.h"

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/findfirst-functions
// used to check file exists only
intptr_t _findfirst(const char* filespec, struct _finddata_t* fileinfo) {
    if (filespec == nullptr) {
        return -1;
    }
    WIN32_FIND_DATAA _finddata;
    auto handle = FindFirstFileA(filespec, &_finddata);
    if (handle != (HANDLE)-1) {
        fileinfo->attrib;
        fileinfo->name;
        fileinfo->size;
        fileinfo->time_access;
        fileinfo->time_create;
        fileinfo->time_write = ((((uint64_t)_finddata.ftLastWriteTime.dwHighDateTime << 32) | (uint64_t)_finddata.ftLastWriteTime.dwLowDateTime) - 116444736000000000ULL) / 10000000ULL;
        return (intptr_t)handle;
    }
    else {
        return -1;
    }
}

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/findclose
// nothing required
int _findclose(intptr_t handle) { 
    if (TRUE == FindClose((HANDLE)handle)){
        return 0;
    }
    return -1;
}

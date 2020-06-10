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
#include <fcntl.h>
#ifndef O_PATH
#define O_PATH 010000000
#endif
#include <sys/statvfs.h>
#include "common_handle.h"
#include "tchar.h"
#include "fileapi.h"

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1) // handleapi.h

#define FILEAPI_TRACE 0

HANDLE FindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) {
    HANDLE ret = INVALID_HANDLE_VALUE;
    if (lpFileName == nullptr) {
        return ret;
    }
    try {
        WIN32_FIND_DATAA findData;
        memset(lpFindFileData, sizeof(WIN32_FIND_DATAW), 0x00);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
        ret = FindFirstFileA(cvt.to_bytes(lpFileName).c_str(), &findData);
        if (lpFindFileData != nullptr) {
            memset(lpFindFileData, 0x00, sizeof(*lpFindFileData));
            auto wsAlternateFileName
                = cvt.from_bytes(std::string(findData.cAlternateFileName));
            auto wsFileName = cvt.from_bytes(std::string(findData.cFileName));
            lpFindFileData->cAlternateFileName;
            lpFindFileData->cFileName;
            u16csncpy(lpFindFileData->cAlternateFileName,
                wsAlternateFileName.data(),
                sizeof(lpFindFileData->cAlternateFileName)
                    / sizeof(lpFindFileData->cAlternateFileName[0]));
            u16csncpy(lpFindFileData->cFileName, wsFileName.data(),
                sizeof(lpFindFileData->cFileName)
                    / sizeof(lpFindFileData->cFileName[0]));
            lpFindFileData->dwFileAttributes = findData.dwFileAttributes;
            lpFindFileData->ftCreationTime = findData.ftCreationTime;
            lpFindFileData->ftLastAccessTime = findData.ftLastAccessTime;
            lpFindFileData->ftLastWriteTime = findData.ftLastWriteTime;
            lpFindFileData->nFileSizeHigh = findData.nFileSizeHigh;
            lpFindFileData->nFileSizeLow = findData.nFileSizeLow;
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return ret;
}

HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    HANDLE ret = INVALID_HANDLE_VALUE;
    // handle used for checking exists only
    if (lpFileName == nullptr) {
        return ret;
    }
    std::string sFileName;
    try {
        sFileName = std::string(lpFileName);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    WIN32_FIND_DATAA findFileDataDummy;
    if (lpFindFileData == nullptr) {
        lpFindFileData = &findFileDataDummy;
    }
    memset(lpFindFileData, 0x00, sizeof(*lpFindFileData));
    try {
        sFileName = std::regex_replace(sFileName, std::regex(u8"\\\\"), u8"/");
        std::string foundFilePath;
        if (sFileName.find_first_of(u8"*") != sFileName.npos
            || sFileName.find_first_of(u8"?") != sFileName.npos) {
            auto dir = boost::filesystem::path(sFileName).parent_path();
            if (dir.string().find_first_of(u8"*") != sFileName.npos
                || dir.string().find_first_of(u8"?") != sFileName.npos) {
                BOOST_LOG_TRIVIAL(warning)
                    << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                    << u8"not implemented";
            }
            else {
                auto sLeafMatch
                    = boost::filesystem::path(sFileName).leaf().string();
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\."), u8"\\.");
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\*"), u8".*");
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\?"), u8".");
                boost::filesystem::directory_iterator endIter;
                std::vector<std::string> paths;
                for (boost::filesystem::directory_iterator iter(dir);
                     iter != endIter; iter++) {
                    paths.push_back(iter->path().string());
                }
                std::sort(paths.begin(), paths.end());
                for (auto tpath : paths) {
                    if (std::regex_match(
                            boost::filesystem::path(tpath).leaf().string(),
                            std::regex(sLeafMatch))) {
                        foundFilePath = tpath;
                        break;
                    }
                }
            }
        }
        else {
            foundFilePath = sFileName;
        }
        if (!foundFilePath.empty()) {
            if (boost::filesystem::exists(foundFilePath)) {
                ret = (HANDLE)__compat_handle_reg_fd(
                    open(foundFilePath.c_str(), O_PATH), foundFilePath,
                    sFileName);
                auto sLeaf
                    = boost::filesystem::path(foundFilePath).leaf().string();
                strncpy(lpFindFileData->cAlternateFileName, sLeaf.c_str(),
                    sizeof(lpFindFileData->cAlternateFileName));
                strncpy(lpFindFileData->cFileName, sLeaf.c_str(),
                    sizeof(lpFindFileData->cFileName));
                lpFindFileData->dwFileAttributes = 0;
                // posix api provides last write time only
                lpFindFileData->ftCreationTime = { 0, 0 };
                lpFindFileData->ftLastAccessTime = { 0, 0 };
                auto lastWriteTime
                    = boost::filesystem::last_write_time(foundFilePath);
                uint64_t lastWriteTimeFileTimeValue
                    = (uint64_t)lastWriteTime * 10000000ULL
                    + 116444736000000000ULL;
                // little endian only
                lpFindFileData->ftLastWriteTime = {
                    (uint32_t)lastWriteTimeFileTimeValue,
                    (uint32_t)(lastWriteTimeFileTimeValue >> 32),
                };
                if (boost::filesystem::is_directory(boost::filesystem::path(foundFilePath))){
                	lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                }
                if (boost::filesystem::is_regular_file(foundFilePath)) {
                    auto sz = boost::filesystem::file_size(foundFilePath);
                    // little endian only
                    lpFindFileData->nFileSizeLow = (uint32_t)sz;
                    lpFindFileData->nFileSizeHigh = (uint32_t)(sz >> 32);
                    lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
                }
            }
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }

#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                            << __LINE__  << ":File=" << lpFileName << "; cFileName=" << lpFindFileData->cFileName;
#endif


    return ret;
}

BOOL FindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) {
    if (lpFindFileData == nullptr) {
        return FALSE;
    }
    WIN32_FIND_DATAA findFileDataA;
    memset(lpFindFileData, sizeof(WIN32_FIND_DATAW), 0x00);
    auto ret = FindNextFileA(hFindFile, &findFileDataA);

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    try{
        if( ret == TRUE){
            u16csncpy(lpFindFileData->cAlternateFileName,
                cvt.from_bytes(findFileDataA.cAlternateFileName).data(),
                sizeof(lpFindFileData->cAlternateFileName)
                    / sizeof(lpFindFileData->cAlternateFileName[0]));
            u16csncpy(lpFindFileData->cFileName,
                cvt.from_bytes(findFileDataA.cFileName).data(),
                sizeof(lpFindFileData->cFileName)
                    / sizeof(lpFindFileData->cFileName[0]));
            lpFindFileData->dwFileAttributes = findFileDataA.dwFileAttributes;
            lpFindFileData->dwReserved0 = findFileDataA.dwReserved0;
            lpFindFileData->dwReserved1 = findFileDataA.dwReserved1;
            lpFindFileData->ftCreationTime = findFileDataA.ftCreationTime;
            lpFindFileData->ftLastAccessTime = findFileDataA.ftLastAccessTime;
            lpFindFileData->ftLastWriteTime = findFileDataA.ftLastWriteTime;
            lpFindFileData->nFileSizeHigh = findFileDataA.nFileSizeHigh;
            lpFindFileData->nFileSizeLow = findFileDataA.nFileSizeLow;
        }
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return ret;
}

BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) {
    BOOL ret = FALSE;
    // handle used for checking exists only
    if (__compat_handle_get_fd_findPattern((int)hFindFile).empty()) {
        return ret;
    }
    std::string sFileName;
    try {
        sFileName = __compat_handle_get_fd_findPattern((int)hFindFile);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    WIN32_FIND_DATAA findFileDataDummy;
    if (lpFindFileData == nullptr) {
        lpFindFileData = &findFileDataDummy;
    }
    try {
        sFileName = std::regex_replace(sFileName, std::regex(u8"\\\\"), u8"/");
        std::string foundFilePath;
        bool lastFound = false;
        if (sFileName.find_first_of(u8"*") != sFileName.npos
            || sFileName.find_first_of(u8"?") != sFileName.npos) {
            auto dir = boost::filesystem::path(sFileName).parent_path();
            if (dir.string().find_first_of(u8"*") != sFileName.npos
                || dir.string().find_first_of(u8"?") != sFileName.npos) {
                BOOST_LOG_TRIVIAL(warning)
                    << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                    << u8"not implemented";
            }
            else {
                auto sLeafMatch
                    = boost::filesystem::path(sFileName).leaf().string();
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\."), u8"\\.");
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\*"), u8".*");
                sLeafMatch = std::regex_replace(
                    sLeafMatch, std::regex(u8"\\?"), u8".");
                boost::filesystem::directory_iterator endIter;
                std::vector<std::string> paths;
                for (boost::filesystem::directory_iterator iter(dir);
                     iter != endIter; iter++) {
                    paths.push_back(iter->path().string());
                }
                std::sort(paths.begin(), paths.end());
                for (auto tpath : paths) {
                    if (!lastFound) {
                        if (tpath
                            == __compat_handle_get_fd_path((int)hFindFile)) {
                            lastFound = true;
                            close(__compat_handle_get_fd((int)hFindFile));
                        }
                    }
                    else {
                        if (std::regex_match(
                                boost::filesystem::path(tpath).leaf().string(),
                                std::regex(sLeafMatch))) {
                            foundFilePath = tpath;
                            break;
                        }
                    }
                }
            }
        }
        else {
            foundFilePath = sFileName;
        }
        if (!foundFilePath.empty() && lastFound) {
            if (boost::filesystem::exists(foundFilePath)) {
                __compat_handle_update_fd((int)hFindFile,
                    open(foundFilePath.c_str(), O_PATH), foundFilePath);
                ret = TRUE;
                auto sLeaf = boost::filesystem::path(foundFilePath).leaf();
                strncpy(lpFindFileData->cAlternateFileName, sLeaf.c_str(),
                    sizeof(lpFindFileData->cAlternateFileName));
                strncpy(lpFindFileData->cFileName, sLeaf.c_str(),
                    sizeof(lpFindFileData->cFileName));
                lpFindFileData->dwFileAttributes = 0;
                // posix api provides last write time only
                lpFindFileData->ftCreationTime = { 0, 0 };
                lpFindFileData->ftLastAccessTime = { 0, 0 };
                auto lastWriteTime
                    = boost::filesystem::last_write_time(foundFilePath);
                uint64_t lastWriteTimeFileTimeValue
                    = (uint64_t)lastWriteTime * 10000000ULL
                    + 116444736000000000ULL;
                // little endian only
                lpFindFileData->ftLastWriteTime = {
                    (uint32_t)lastWriteTimeFileTimeValue,
                    (uint32_t)(lastWriteTimeFileTimeValue >> 32),
                };
                if (boost::filesystem::is_directory(foundFilePath)) {
                    lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                }
                if (boost::filesystem::is_regular_file(foundFilePath)) {
                    auto sz = boost::filesystem::file_size(foundFilePath);
                    // little endian only
                    lpFindFileData->nFileSizeLow = (uint32_t)sz;
                    lpFindFileData->nFileSizeHigh = (uint32_t)(sz >> 32);
                }
            }
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }

#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                            << __LINE__  << ":File=" << __compat_handle_get_fd_findPattern((int)hFindFile);;
#endif

    return ret;
}

BOOL FindClose(HANDLE hFindFile) {
    BOOL ret = FALSE;
    auto fd = __compat_handle_get_fd((int)hFindFile);
    try {
        if (fd != -1) {
            close(fd);
            ret = TRUE;
        }
        __compat_handle_erase((int)hFindFile);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                    << " " << __LINE__ << " " << e.what();
    }
    return ret;
}

HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    auto ret = INVALID_HANDLE_VALUE;
    if (lpFileName != nullptr) {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            auto sFileName = cvt.to_bytes(std::u16string(lpFileName));
            ret = CreateFileA(sFileName.c_str(), dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile);
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    // TODO: COM PORTS, GPIOS, ASYNC IO...
    auto ret = INVALID_HANDLE_VALUE;
    if (lpFileName != nullptr) {
        try {
            auto sFilePath = std::string(lpFileName);
            int flags = 0;
            mode_t mode = 0;
            bool paramsValid = true;
            if (!boost::filesystem::exists(
                    boost::filesystem::path(sFilePath).parent_path())) {
                boost::filesystem::create_directories(
                    boost::filesystem::path(sFilePath).parent_path());
            }
            if ((dwDesiredAccess & GENERIC_READ)
                && (dwDesiredAccess & GENERIC_WRITE)) {
                flags |= O_RDWR;
            }
            else if (dwDesiredAccess & GENERIC_WRITE) {
                flags |= O_WRONLY;
            }
            else {
                flags |= O_RDONLY;
            }
            switch (dwCreationDisposition) {
            case CREATE_ALWAYS:
                if (boost::filesystem::exists(sFilePath)) {
                    boost::filesystem::remove(sFilePath);
                }
                flags |= O_CREAT;
                mode = S_IRUSR | S_IWUSR; // 600

                break;
            case CREATE_NEW:
                break;
            case OPEN_ALWAYS:
                if (!boost::filesystem::exists(sFilePath)) {
                    flags |= O_CREAT;
                    mode = S_IRUSR | S_IWUSR; // 600
                }
                break;
            case OPEN_EXISTING:
                if (!boost::filesystem::exists(sFilePath)) {
                    paramsValid = false;
                }
                break;
            case TRUNCATE_EXISTING:
                if (!boost::filesystem::exists(sFilePath)) {
                    paramsValid = false;
                }
                else {
                    flags |= O_TRUNC;
                }
                break;
            default:
                paramsValid = false;
                break;
            }

            if (dwFlagsAndAttributes & FILE_FLAG_WRITE_THROUGH) {
                flags |= O_SYNC;
            }
            else if (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING) {
                flags |= O_DIRECT;
            }
            else {
                
            }

            if (paramsValid) {
                auto fd = open(sFilePath.c_str(), flags, mode);
                ret = (HANDLE)__compat_handle_reg_fd(fd, sFilePath);
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << "; lpFileName = " <<lpFileName <<"; e=" << e.what();
        }
    }
    return ret;
}

BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    // TODO: COM PORTS, GPIOS, ASYNC IO...
    auto ret = FALSE;
    if (lpNumberOfBytesRead != nullptr) {
        *lpNumberOfBytesRead = 0;
    }
    auto fd = __compat_handle_get_fd((int)hFile);
    if (lpBuffer != nullptr && (HANDLE)fd != INVALID_HANDLE_VALUE) {
        auto bytesRead = read(fd, lpBuffer, nNumberOfBytesToRead);
        if (bytesRead != -1 && lpNumberOfBytesRead != nullptr) {
            *lpNumberOfBytesRead = bytesRead;
        }
        if (bytesRead >= -1) {
        	ret = TRUE;
        }
    }

#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                                << __LINE__ << ":HANDLE=" << (void*)hFile << ":" << __compat_handle_get_fd_path((int)hFile);
#endif

    return ret;
}

BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
    // TODO: COM PORTS, GPIOS, ASYNC IO...
    // TODO: not implemented (used in COM ports only)
    auto ret = FALSE;
    if (lpNumberOfBytesWritten != nullptr) {
        *lpNumberOfBytesWritten = 0;
    }
    auto fd = __compat_handle_get_fd((int)hFile);
    if (lpBuffer != nullptr && (HANDLE)fd != INVALID_HANDLE_VALUE) {
        auto bytesWrite = write(fd, lpBuffer, nNumberOfBytesToWrite);
        if (bytesWrite != -1 && lpNumberOfBytesWritten != nullptr) {
            *lpNumberOfBytesWritten = bytesWrite;
        }
        if( bytesWrite > -1 ){
        	ret = TRUE;
        }
    }

#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                                << __LINE__ << ":HANDLE=" << (void*)hFile << ":" << __compat_handle_get_fd_path((int)hFile);
#endif

    return ret;
}

DWORD GetFileAttributesW(LPCWSTR lpFileName) {
    auto ret = INVALID_FILE_ATTRIBUTES;
    if (lpFileName != nullptr) {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            auto wsFilePath = std::u16string(lpFileName);
            ret = GetFileAttributesA(cvt.to_bytes(wsFilePath).c_str());
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

DWORD GetFileAttributesA(LPCSTR lpFileName) {
    auto ret = INVALID_FILE_ATTRIBUTES;
    if (lpFileName != nullptr) {
        try {
            auto sFilePath = std::string(lpFileName);
            if (boost::filesystem::exists(sFilePath)) {
                ret = 0;
                if (boost::filesystem::is_directory(sFilePath)) {
                    ret |= FILE_ATTRIBUTE_DIRECTORY;
                }
                if (boost::filesystem::is_symlink(sFilePath)) {
                    ret |= FILE_ATTRIBUTE_REPARSE_POINT;
                }
                if (boost::filesystem::is_regular_file(sFilePath)) {
                    ret = FILE_ATTRIBUTE_NORMAL;
                }
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

BOOL CreateDirectoryW(
    LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    auto ret = FALSE;
    if (lpPathName != nullptr) {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            auto wsPathName = std::u16string(lpPathName);
            ret = CreateDirectoryA(
                cvt.to_bytes(wsPathName).c_str(), lpSecurityAttributes);
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

BOOL CreateDirectoryA(
    LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    auto ret = FALSE;
    if (lpPathName != nullptr) {
        try {
            auto sPathName = std::string(lpPathName);
            BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << sPathName;
            if (!boost::filesystem::exists(sPathName)) {
                if (boost::filesystem::create_directories(sPathName)) {
                    ret = TRUE;
                }
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

BOOL RemoveDirectoryW(LPCWSTR lpPathName) {
    auto ret = FALSE;
    if (lpPathName != nullptr) {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            auto wsPathName = std::u16string(lpPathName);
            ret = RemoveDirectoryA(cvt.to_bytes(wsPathName).c_str());
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }

    return ret;
}

BOOL RemoveDirectoryA(LPCSTR lpPathName) {
    auto ret = FALSE;
    if (lpPathName != nullptr) {
        try {
            auto sPathName = std::string(lpPathName);
            if (boost::filesystem::exists(sPathName)
                && boost::filesystem::is_directory(sPathName)) {
                if (boost::filesystem::directory_iterator(sPathName)
                    == boost::filesystem::directory_iterator()) {
                    if (boost::filesystem::remove(sPathName)) {
                        ret = TRUE;
                    }
                }
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }

#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                            << __LINE__ << ":Path=" << lpPathName;
#endif

    return ret;
}

BOOL DeleteFileW(LPCWSTR lpFileName) {
    auto ret = FALSE;
    if (lpFileName != nullptr) {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            auto wsPathName = std::u16string(lpFileName);
            ret = DeleteFileA(cvt.to_bytes(wsPathName).c_str());
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }

    return ret;
}

BOOL DeleteFileA(LPCSTR lpFileName) {
    auto ret = FALSE;
    if (lpFileName != nullptr) {
        try {
            auto sPathName = std::string(lpFileName);
            if (boost::filesystem::exists(sPathName)
                && (boost::filesystem::is_regular_file(sPathName)
                    || boost::filesystem::is_symlink(sPathName))) {
                if (boost::filesystem::remove(sPathName)) {
                    ret = TRUE;
                }else{
                    ret = FALSE;
                }
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }


#if FILEAPI_TRACE
    BOOST_LOG_TRIVIAL(error) << __FILE__ << ":" << __FUNCTION__ << ":"
                            << __LINE__ << ":File=" << lpFileName;
#endif

    return ret;
}

BOOL GetDiskFreeSpaceExW(LPCWSTR lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes) {
    auto ret = FALSE;
    std::string sPathName;
    if (lpDirectoryName == nullptr) {
        sPathName = boost::filesystem::current_path().root_directory().string();
    }
    else {
        try {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            sPathName = cvt.to_bytes(std::u16string(lpDirectoryName));
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    try {
        ret = GetDiskFreeSpaceExA(sPathName.c_str(),
            lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes,
            lpTotalNumberOfFreeBytes);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return ret;
}

BOOL GetDiskFreeSpaceExA(LPCSTR lpDirectoryName,
    PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes,
    PULARGE_INTEGER lpTotalNumberOfFreeBytes) {
    auto ret = FALSE;
    std::string sPathName;
    if (lpDirectoryName == nullptr) {
        sPathName = boost::filesystem::current_path().root_directory().string();
    }
    else {
        try {
            sPathName = std::string(lpDirectoryName);
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    try {
        struct statvfs resultInternal;
        if (statvfs(sPathName.c_str(), &resultInternal) != -1) {
            if (lpFreeBytesAvailableToCaller != nullptr) {
                lpFreeBytesAvailableToCaller->QuadPart
                    = (uint64_t)resultInternal.f_bsize * (uint64_t)resultInternal.f_bavail;
            }
            if (lpTotalNumberOfBytes != nullptr) {
                lpTotalNumberOfBytes->QuadPart
                    = (uint64_t)resultInternal.f_bsize * (uint64_t)resultInternal.f_blocks;
            }
            if (lpTotalNumberOfFreeBytes != nullptr) {
                lpTotalNumberOfFreeBytes->QuadPart
                    = (uint64_t)resultInternal.f_bsize * (uint64_t)resultInternal.f_bfree;
            }
            ret = TRUE;
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return ret;
}

BOOL FlushFileBuffers(HANDLE hFile) {
    auto fd = __compat_handle_get_fd((int)hFile);
    if ((HANDLE)fd != INVALID_HANDLE_VALUE) {
        syncfs(fd);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

// lpDistanceToMoveHigh ignored, always NULL
DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) {
    auto fd = __compat_handle_get_fd((int)hFile);
    if ((HANDLE)fd != INVALID_HANDLE_VALUE) {
        // dwMoveMethod -> whence directly
        return lseek(fd, lDistanceToMove, dwMoveMethod);
    }
    else {
        return FALSE;
    }
}

// little endian
BOOL FileTimeToLocalFileTime(
    const FILETIME* lpFileTime, LPFILETIME lpLocalFileTime) {
    if (lpFileTime == nullptr || lpLocalFileTime == nullptr
        || lpLocalFileTime == lpFileTime) {
        return FALSE;
    }
    int64_t ttmp = ((int64_t)lpFileTime->dwHighDateTime) << (sizeof(DWORD) * 8)
        | (int64_t)lpFileTime->dwLowDateTime;

    auto utc_now = boost::posix_time::second_clock::universal_time();
    auto local_now = boost::date_time::c_local_adjustor<
        boost::posix_time::ptime>::utc_to_local(utc_now);
    auto offset = (local_now - utc_now).total_nanoseconds() / 100LL;
    ttmp = ttmp + offset;
    lpLocalFileTime->dwLowDateTime = ttmp & (1LL << (sizeof(DWORD) * 8) - 1);
    lpLocalFileTime->dwHighDateTime
        = (ttmp - lpLocalFileTime->dwLowDateTime) >> (sizeof(DWORD) * 8);
    return TRUE;
}

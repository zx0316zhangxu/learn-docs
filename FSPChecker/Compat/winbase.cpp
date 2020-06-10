#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <locale>
#include <codecvt>
#include "Winbase.h"

BOOL CopyFileW(
    LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists) {
    if (lpExistingFileName == 0 || lpNewFileName == 0) {
        return FALSE;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    return CopyFileA(cvt.to_bytes(std::u16string(lpExistingFileName)).c_str(),
        cvt.to_bytes(std::u16string(lpNewFileName)).c_str(), bFailIfExists);
}

BOOL MoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName) {
    if (lpExistingFileName == 0 || lpNewFileName == 0) {
        return FALSE;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    return MoveFileA(cvt.to_bytes(std::u16string(lpExistingFileName)).c_str(),
        cvt.to_bytes(std::u16string(lpNewFileName)).c_str());
}

// TODO: not implemented
DWORD GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer) {
    return FALSE;
}

// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfile
BOOL CopyFileA(
    LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {
    if (lpExistingFileName == 0 || lpNewFileName == 0) {
        return FALSE;
    }
// TODO: memory boundary check
    auto srcFileName = std::string(lpExistingFileName);
    auto dstFileName = std::string(lpNewFileName);
    if (!boost::filesystem::exists(srcFileName)) {
        return FALSE;
    }
    if (boost::filesystem::exists(dstFileName)) {
        if (bFailIfExists == TRUE) {
            return FALSE;
        }
        else {
            boost::filesystem::remove(dstFileName);
        }
    }
    boost::filesystem::copy(srcFileName, dstFileName);
    return TRUE;
}

// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefile
BOOL MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName) {
    if (lpExistingFileName == 0 || lpNewFileName == 0) {
        return FALSE;
    }
    // TODO: memory boundary check
    auto srcFileName = std::string(lpExistingFileName);
    auto dstFileName = std::string(lpNewFileName);
    if (!boost::filesystem::exists(lpExistingFileName)
        || boost::filesystem::exists(lpNewFileName)) {
        return FALSE;
    }
    else {
        boost::filesystem::copy(lpExistingFileName, lpNewFileName);
        boost::filesystem::remove(lpExistingFileName);
        return TRUE;
    }
}

// TODO: not implemented
DWORD GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer) {
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;

    return FALSE;
}

void GlobalMemoryStatus(
  LPMEMORYSTATUS lpBuffer
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;

}


BOOL SetFileAttributesW(
  LPCTSTR lpFileName, 
  DWORD dwFileAttributes
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;

    return FALSE;

}

BOOL SetFileAttributesA(
  LPCTSTR lpFileName, 
  DWORD dwFileAttributes
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;

    return FALSE;

}

#if 0 // not used

// TODO: not implemented
BOOL GetCommState(HANDLE hFile, LPDCB lpDCB) { return FALSE; }

// TODO: not implemented
BOOL SetCommState(HANDLE hFile, LPDCB lpDCB) { return FALSE; }

// TODO: not implemented
BOOL GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts) {
    return FALSE;
}

// TODO: not implemented
BOOL SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts) {
    return FALSE;
}

// TODO: not implemented
BOOL PurgeComm(HANDLE hFile, DWORD dwFlags) { return FALSE; }

#endif

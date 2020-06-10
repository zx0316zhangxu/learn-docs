#include <boost/log/trivial.hpp>
#include "wingdi.h"

int AddFontResourceA(
  LPCSTR Arg1
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;
    return NULL;

}

int AddFontResourceW(
  LPCWSTR Arg1
){
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;
    return NULL;

}


HDC GetDC(HWND) {
    return 0;
}

BOOL WINAPI DeleteDC(HDC) {
    return 0;
}

BOOL DeleteObject(HGDIOBJ ho) {
    return 0;
}

int ReleaseDC(HWND,HDC) {
    return 0;
}

HDC WINAPI CreateCompatibleDC(HDC) {
    return 0;    
}

HGDIOBJ WINAPI GetStockObject(int obj) {
    return (HGDIOBJ)obj;
}

int WINAPI FillRect(HDC,LPCRECT,HBRUSH) {
    return 0;
}

HGDIOBJ WINAPI SelectObject(HDC,HGDIOBJ obj) {
    return 0;
}

int WINAPI GetObjectA(HGDIOBJ h,int sz,PVOID ptr) {
    return 0;
}

int WINAPI GetObjectW(HGDIOBJ h,int sz,PVOID obj) {
    return 0;
}

BOOL WINAPI ExtTextOutA(HDC,int,int,UINT,LPCRECT,LPCSTR,UINT,const INT*) {
    return 0;
}

BOOL WINAPI ExtTextOutW(HDC,int,int,UINT,LPCRECT,LPCWSTR,UINT,const INT*) {
    return 0;
}

HBITMAP WINAPI CreateDIBSection(HDC,const BITMAPINFO*,UINT,void**,HANDLE,DWORD) {
    return 0;
}

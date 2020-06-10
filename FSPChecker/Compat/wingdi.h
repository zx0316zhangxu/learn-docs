#ifndef __WINGDI_H__
#define __WINGDI_H__

#include "minwindef.h"


#define BI_RGB 0
#define WHITE_BRUSH 0
#define DIB_RGB_COLORS 0

DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HGDIOBJ);

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

typedef struct tagRECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;
typedef const RECT *LPCRECT;

typedef struct tagBITMAP {
  LONG   bmType;
  LONG   bmWidth;
  LONG   bmHeight;
  LONG   bmWidthBytes;
  WORD   bmPlanes;
  WORD   bmBitsPixel;
  LPVOID bmBits;
} BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int AddFontResourceA(
  LPCSTR Arg1
);

int AddFontResourceW(
  LPCWSTR Arg1
);

// WINGDI SECTION
HDC GetDC(HWND hWnd);
BOOL WINAPI DeleteDC(HDC);
BOOL DeleteObject(HGDIOBJ ho);
int ReleaseDC(HWND hWnd,HDC hDC);
HDC WINAPI CreateCompatibleDC(HDC);
HGDIOBJ WINAPI GetStockObject(int);
int WINAPI FillRect(HDC,LPCRECT,HBRUSH);
HGDIOBJ WINAPI SelectObject(HDC,HGDIOBJ);
int WINAPI GetObjectA(HGDIOBJ,int,PVOID);
int WINAPI GetObjectW(HGDIOBJ,int,PVOID);
#ifndef UNICODE
#define GetObject GetObjectA
#else
#define GetObject GetObjectW
#endif
BOOL WINAPI ExtTextOutA(HDC,int,int,UINT,LPCRECT,LPCSTR,UINT,const INT*);
BOOL WINAPI ExtTextOutW(HDC,int,int,UINT,LPCRECT,LPCWSTR,UINT,const INT*);
#ifndef UNICODE
#define ExtTextOut ExtTextOutA
#else
#define ExtTextOut ExtTextOutW
#endif
HBITMAP WINAPI CreateDIBSection(HDC,const BITMAPINFO*,UINT,void**,HANDLE,DWORD);

// GDIP SECTION
void GdipCreateBitmapFromFile(TCHAR *filename, void** bitmap);
void GdipCreateHBITMAPFromBitmap(void* bitmap, HBITMAP* hbitmap, RGBQUAD background);
void GdipDisposeImage(HBITMAP hbitmap);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
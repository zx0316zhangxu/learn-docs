#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <stdarg.h>
#include <cstdio>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <wctype.h>
#include <iconv.h>
#include <errno.h>
#define NOMINMAX
#include "tchar.h"
#include "windows.h"

#define HEAP_WORK_SIZE (2048)

errno_t _u16splitpath_s(const char16_t* path, char16_t* drive,
    size_t driveNumberOfElements, char16_t* dir, size_t dirNumberOfElements,
    char16_t* fname, size_t nameNumberOfElements, char16_t* ext,
    size_t extNumberOfElements) {
    if (path == nullptr) {
        return EINVAL;
    }
    if (drive == nullptr && driveNumberOfElements != 0) {
        return EINVAL;
    }
    if (drive != nullptr && driveNumberOfElements == 0) {
        return EINVAL;
    }
    if (dir == nullptr && dirNumberOfElements != 0) {
        return EINVAL;
    }
    if (dir != nullptr && dirNumberOfElements == 0) {
        return EINVAL;
    }
    if (fname == nullptr && nameNumberOfElements != 0) {
        return EINVAL;
    }
    if (fname != nullptr && nameNumberOfElements == 0) {
        return EINVAL;
    }
    if (ext == nullptr && extNumberOfElements != 0) {
        return EINVAL;
    }
    if (ext != nullptr && extNumberOfElements == 0) {
        return EINVAL;
    }
    if (drive != nullptr) {
        drive[0] = 0;
    }
    if (dir != nullptr) {
        dir[0] = 0;
    }
    if (fname != nullptr) {
        fname[0] = 0;
    }
    if (ext != nullptr) {
        ext[0] = 0;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    auto wpath = std::u16string(path);
    auto spath = cvt.to_bytes(wpath);
    auto bpath = boost::filesystem::path(spath);
    if (drive != nullptr && bpath.has_root_path()) {
        auto rootPath = bpath.root_path();
        if (rootPath.string().length() + 1 > driveNumberOfElements) {
            return ERANGE;
        }
        else {
            u16cscpy(drive, cvt.from_bytes(rootPath.string()).data());
        }
    }
    if (dir != nullptr && bpath.has_parent_path()) {
        auto parentPath = bpath.parent_path();
        if (parentPath.string().length() + 1 > dirNumberOfElements) {
            return ERANGE;
        }
        else {
            u16cscpy(dir, cvt.from_bytes(parentPath.string()).data());
        }
    }
    if (fname != nullptr && bpath.has_filename()) {
        auto filename = bpath.stem();
        if (filename.string().length() + 1 > nameNumberOfElements) {
            return ERANGE;
        }
        else {
            u16cscpy(fname, cvt.from_bytes(filename.string()).data());
        }
    }
    if (ext != nullptr && bpath.has_extension()) {
        auto extension = bpath.extension();
        if (extension.string().length() + 1 > extNumberOfElements) {
            return ERANGE;
        }
        else {
            u16cscpy(ext, cvt.from_bytes(extension.string()).data());
        }
    }
    return 0;
}

char16_t* u16csncpy(char16_t* dst, const char16_t* src, size_t n) {
    if (dst != nullptr && src != nullptr && n > 0) {
        size_t cnt = 0;
        while (cnt < n) {
            dst[cnt] = src[cnt];
            if (src[cnt] == 0) {
                break;
            }
            cnt++;
        }
    }
    return dst;
}

size_t u16csnlen(const char16_t* str, size_t __n) {
    if (str == nullptr) {
        return 0;
    }
    size_t ret = (size_t)-1;
    size_t cursor = 0;
    while (cursor < __n) {
        if (str[cursor] == 0) {
            break;
        }
        cursor++;
        ret = cursor;
    }
    return ret;
}

size_t u16cslen(const char16_t* str) { return u16csnlen(str, (size_t)-1); }

char16_t* u16cscpy(char16_t* destination, const char16_t* source) {
    return u16csncpy(destination, source, (size_t)-1);
}

char16_t* u16csstr(const char16_t* wcs1, const char16_t* wcs2) {
    if (wcs1 != nullptr && wcs2 != nullptr) {
        auto cklen = u16cslen(wcs2);
        while (*wcs1 != 0) {
            if (u16csncmp(wcs1, wcs2, cklen) == 0) {
                return (char16_t*)wcs1;
            }
            wcs1++;
        }
    }
    return nullptr;
}

char16_t* u16cscat(char16_t* destination, const char16_t* source) {
    if (destination != nullptr && source != nullptr) {
        auto cursor = destination;
        while (*cursor != 0) {
            cursor++;
        }
        u16cscpy(cursor, source);
    }
    return destination;
}

int u16csncmp(const char16_t* wcs1, const char16_t* wcs2, size_t num) {
    int ret = 0;
    if (wcs1 == nullptr || wcs2 == nullptr) {
        return _NLSCMPERROR;
    }
    for (size_t i = 0; i < num; i++) {
        ret = wcs1[i] - wcs2[i];
        if (ret != 0 || wcs1[i] == 0 || wcs2[i] == 0) {
            break;
        }
    }
    return ret;
}

int u16cscmp(const char16_t* wcs1, const char16_t* wcs2) {
    return u16csncmp(wcs1, wcs2, (size_t)-1);
}

char16_t* u16cstok(char16_t* wcs, const char16_t* delimiters) {
    static char16_t* lastcursor = nullptr;
    auto num_delimiters = u16cslen(delimiters);
    if (wcs) {
        lastcursor = wcs;
    }
    if (!lastcursor) {
        return nullptr;
    }
    if (num_delimiters == 0) {
        return lastcursor;
    }
    while (*lastcursor != 0x0000) {
        auto b_found = false;
        for (size_t i = 0; i < num_delimiters; i++) {
            if (*lastcursor == delimiters[i]) {
                b_found = true;
                break;
            }
        }
        if (b_found) {
            lastcursor++;
        }
        else {
            break;
        }
    }
    if (*lastcursor == 0x0000) {
        return nullptr;
    }
    auto endcursor = lastcursor;
    while (*endcursor != 0x0000) {
        auto b_found = false;
        for (size_t i = 0; i < num_delimiters; i++) {
            if (*endcursor == delimiters[i]) {
                b_found = true;
                break;
            }
        }
        if (b_found) {
            break;
        }
        else {
            endcursor++;
        }
    }
    static char16_t* buffer = nullptr;
    static size_t buffer_size = 0;
    auto tsize = (size_t)(endcursor - lastcursor + 1);
    if (buffer_size < tsize) {
        if (buffer) {
            delete[] buffer;
        }
        buffer = new char16_t[tsize];
        buffer_size = tsize;
    }
    buffer[tsize - 1] = 0;
    memcpy(buffer, lastcursor, (tsize - 1) * sizeof(char16_t));
    lastcursor = endcursor;
    return buffer;
}

int _u16csicmp(const char16_t* string1, const char16_t* string2) {
    int ret = 0;
    if (string1 == 0 || string2 == 0) {
        return _NLSCMPERROR;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    auto nwstr1 = std::u16string(string1);
    auto nwstr2 = std::u16string(string2);
    auto nstr1 = cvt.to_bytes(nwstr1);
    auto nstr2 = cvt.to_bytes(nwstr2);
    std::transform(nstr1.begin(), nstr1.end(), nstr1.begin(), ::tolower);
    std::transform(nstr2.begin(), nstr2.end(), nstr2.begin(), ::tolower);
    ret = strcmp(nstr1.c_str(), nstr2.c_str());
    return ret;
}

int vsu16printf(
    char16_t* u16buffer, size_t maxlen, const char16_t* u16format, va_list va) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    if (u16buffer) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
        char* tmp = new char[HEAP_WORK_SIZE*2];
        auto u16format_str = cvt.to_bytes(u16format);
        auto fmt = u16format_str.c_str();
        auto s = tmp;

        std::string ts;
        for (; *fmt; fmt++) {
            if (*fmt == '%') {
                switch (*++fmt) {
                case '0': {
                    auto append_len = 0;
                    char str[16];
                    int  i = 0;
                    while(isdigit(*fmt)){
                        str[i++] = *fmt;
                        fmt ++;
                    }
                    append_len = atoi(str);

                    switch(*fmt){
                        case 'd':
                            /* decimal int */
                            ts = (boost::format("%1%") % va_arg(va, int)).str();
                            if(append_len > ts.length()){
                                memset(s, '0', append_len);
                                s += append_len-ts.length();
                            }
                            strcpy(s, ts.c_str());
                            s += ts.length();
                            break;

                        case 'x':
                            /* hexadecimal int */
                            ts = (boost::format("%1%")
                                % boost::io::group(std::hex, va_arg(va, int)))
                                    .str();
                            if(append_len > ts.length()){
                                memset(s, '0', append_len);
                                s += append_len-ts.length();
                            }
                            strcpy(s, ts.c_str());
                            s += ts.length();
                            break;

                        case 'X':
                            /* hexadecimal int */
                            ts = (boost::format("%1%")
                                % boost::io::group(std::hex, va_arg(va, int)))
                                    .str();
                            transform(ts.begin(), ts.end(), ts.begin(), ::toupper);
                            if(append_len > ts.length()){
                                memset(s, '0', append_len);
                                s += append_len-ts.length();
                            }
                            strcpy(s, ts.c_str());
                            s += ts.length();
                            break;
                        default:
                            /* error */
                            *s = 0;
                            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
                            return -1;  

                            break;

                    }
                }
                    break;
                case 'c':
                    /* char */
                    *(s++) = (char)va_arg(va, int);
                    break;
                case 'd':
                    /* decimal int */
                    ts = (boost::format("%1%") % va_arg(va, int)).str();
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;
                case 'u':
                    /* unsigned int */
                    ts = (boost::format("%1%") % va_arg(va, unsigned int))
                             .str();
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;
                case 'o':
                    /* octal int */
                    ts = (boost::format("%1%")
                        % boost::io::group(std::oct, va_arg(va, int)))
                             .str();
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;
                case 'x':
                    /* hexadecimal int */
                    ts = (boost::format("%1%")
                        % boost::io::group(std::hex, va_arg(va, int)))
                            .str();
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;

                case 'X':
                    /* hexadecimal int */
                    ts = (boost::format("%1%")
                        % boost::io::group(std::hex, va_arg(va, int)))
                            .str();
                    transform(ts.begin(), ts.end(), ts.begin(), ::toupper);
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;
                case 's':
                    /* string */
                    ts = cvt.to_bytes(std::u16string(va_arg(va, char16_t*)));
                    strcpy(s, ts.c_str());
                    s += ts.length();
                    break;
                case 'l':
                    /* long */
                    switch (*++fmt) {
                    case 'd':
                        /* decimal long */
                        ts = (boost::format("%1%") % va_arg(va, long int))
                                 .str();
                        strcpy(s, ts.c_str());
                        s += ts.length();
                        break;
                    case 'u':
                        /* unsigned long */
                        ts = (boost::format("%1%")
                            % va_arg(va, unsigned long int))
                                 .str();
                        strcpy(s, ts.c_str());
                        s += ts.length();
                        break;
                    case 'o':
                        /* octal long */
                        ts = (boost::format("%1%")
                            % boost::io::group(std::oct, va_arg(va, long int)))
                                 .str();
                        strcpy(s, ts.c_str());
                        s += ts.length();
                        break;
                    case 'x':
                        /* hexadecimal long */
                        ts = (boost::format("%1%")
                            % boost::io::group(std::hex, va_arg(va, long int)))
                                 .str();
                        strcpy(s, ts.c_str());
                        s += ts.length();
                        break;
                    }
                    break;
                case '%':
                    /* % */
                    *(s++) = *fmt;
                    break;
                default:
                    *s = 0;
                    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
                    return -1;
                }
            }
            else {
                *(s++) = *fmt;
            }
        }
        *s = 0;
        u16csncpy(u16buffer, cvt.from_bytes(tmp).data(), maxlen);
        delete[] tmp;
    }
    return 0;
}

void u16printf(const char16_t* format, ...) {
    char16_t *buffer = new char16_t[65536];
    va_list va;
    va_start(va, format);
    const int ret = vsu16printf(buffer, (size_t)65536, format, va);
    va_end(va);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    printf("%s", cvt.to_bytes(buffer).c_str());
    delete[] buffer;
}

int su16printf(char16_t* buffer, size_t len, const char16_t* format, ...) {
    va_list va;
    va_start(va, format);
    const int ret = vsu16printf(buffer, (size_t)-1, format, va);
    va_end(va);
    return ret;
}

long int u16cstol(const char16_t* str, char16_t** endptr, int base){
	auto srcChars = 0;
	srcChars = u16cslen(str) + 1;
    long int ret = 0;

    auto pdst = new char[HEAP_WORK_SIZE];
    if (pdst == 0) {
        // TODO: error code
        return -1;
    }
    try{
        auto cvtPtr = pdst;
        size_t dstSizeRemaining = HEAP_WORK_SIZE;

        size_t srcSizeRemaining = srcChars * sizeof(char16_t);

        iconv_t conveter  = iconv_open("ASCII","UCS-2LE");

        if(conveter!=(iconv_t)(-1)) {
            size_t rc=iconv(conveter,(char**) &str, &srcSizeRemaining, &cvtPtr, &dstSizeRemaining);
            iconv_close(conveter);
        }

        ret = std::stol(pdst, nullptr, base);
        
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << "; len=" << srcChars << "; str=" << str << "; pdst=" << pdst << "e=" << e.what();
    }

	delete[] pdst;

	return ret;
}

// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr,
    int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar,
    LPBOOL lpUsedDefaultChar) {
    if (lpWideCharStr == 0) {
        // TODO: error code
        return -1;
    }
    iconv_t conv = (iconv_t)(-1);
    size_t requiredDstSize = 0;
    switch (CodePage) {
    case CP_ACP:
        conv = iconv_open("", "UCS-2LE");
        break;

    case CP_UTF7:
        conv = iconv_open("UTF-7", "UCS-2LE");
        break;

    case CP_UTF8:
        conv = iconv_open("UTF-8", "UCS-2LE");
        break;

    default:
        // do nothing
        break;
    }

    if (conv == (iconv_t)(-1)) {
        // TODO: error code
        return -1;
    }

    auto srcChars = 0;
    if (cchWideChar == -1) {
        // null terminated
        srcChars = u16cslen(lpWideCharStr) + 1;
    }
    else if (cchWideChar == 0) {
        // TODO: error code
        return -1;
    }
    else {
        // non null terminated
        srcChars = cchWideChar;
    }

    auto ppsrc = (char**)(&lpWideCharStr);
    size_t srcSizeRemaining = srcChars * sizeof(char16_t);
    auto pdst = new char[HEAP_WORK_SIZE];
    if (pdst == 0) {
        // TODO: error code
        return -1;
    }
    auto cvtPtr = pdst;
    size_t dstSizeRemaining = HEAP_WORK_SIZE;

    auto nconv
        = iconv(conv, ppsrc, &srcSizeRemaining, &cvtPtr, &dstSizeRemaining);

    if (nconv == EILSEQ || nconv == EINVAL || nconv == E2BIG) {
        // TODO: error code
        goto ERROR_RETURN;
    }

    if (srcSizeRemaining > 0) {
        // TODO: error code
        goto ERROR_RETURN;
    }

    requiredDstSize = HEAP_WORK_SIZE - dstSizeRemaining;

    if (cbMultiByte == 0) {
        //do nothing
        //return requiredDstSize;
    }
    else if (lpMultiByteStr != 0 && cbMultiByte >= requiredDstSize) {
        memcpy(lpMultiByteStr, pdst, requiredDstSize);
    }
    else {
        // TODO: error code
        goto ERROR_RETURN;
    }

    iconv_close(conv);
    delete[] pdst;
    return requiredDstSize;

ERROR_RETURN:
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
    iconv_close(conv);
    delete[] pdst;
    return -1;
}

// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr,
    int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) {
    if (lpMultiByteStr == 0) {
        // TODO: error code
        return -1;
    }
    iconv_t conv = (iconv_t)(-1);
    size_t requiredDstSize = 0;
    switch (CodePage) {
    case CP_ACP:
        conv = iconv_open("UCS-2LE", "");
        break;

    case CP_UTF7:
        conv = iconv_open("UCS-2LE", "UTF-7");
        break;

    case CP_UTF8:
        conv = iconv_open("UCS-2LE", "UTF-8");
        break;

    default:
        // do nothing
        break;
    }

    if (conv == (iconv_t)(-1)) {
        // TODO: error code
        return -1;
    }

    auto srcBytes = 0;
    if (cbMultiByte == -1) {
        srcBytes = strlen(lpMultiByteStr);
    }
    else if (cchWideChar == 0) {
        // TODO: error code
        return -1;
    }
    else {
        // non null terminated
        srcBytes = cbMultiByte;
    }

    auto ppsrc = (char**)(&lpMultiByteStr);
    size_t srcSizeRemaining = srcBytes;
    auto pdst = new char[HEAP_WORK_SIZE];
    if (pdst == 0) {
        // TODO: error code
        return -1;
    }
    auto cvtPtr = pdst;
    size_t dstSizeRemaining = HEAP_WORK_SIZE;

    auto nconv
        = iconv(conv, ppsrc, &srcSizeRemaining, &cvtPtr, &dstSizeRemaining);

    if (nconv == EILSEQ || nconv == EINVAL || nconv == E2BIG) {
        // TODO: error code
        goto ERROR_RETURN;
    }

    if (srcSizeRemaining > 0) {
        // TODO: error code
        goto ERROR_RETURN;
    }

    requiredDstSize = HEAP_WORK_SIZE - dstSizeRemaining;

    if (cchWideChar == 0) {
        delete[] pdst;
        if (cbMultiByte >= 0) {
            return requiredDstSize / sizeof(char16_t);
        }
        else {
            return requiredDstSize / sizeof(char16_t) + 1;
        }
    }
    else if (lpWideCharStr != 0
        && cchWideChar * sizeof(char16_t) >= requiredDstSize) {
        memcpy(lpWideCharStr, pdst, requiredDstSize);
    }
    else {
        // TODO: error code
        goto ERROR_RETURN;
    }

    iconv_close(conv);
    delete[] pdst;

    if (cbMultiByte >= 0) {
        return requiredDstSize / sizeof(char16_t);
    }
    else {
        return requiredDstSize / sizeof(char16_t) + 1;
    }

ERROR_RETURN:
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
    delete[] pdst;
    return -1;

}


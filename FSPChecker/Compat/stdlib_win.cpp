#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stack>
#include <locale>
#include <iostream>
#include <codecvt>
#include <string>
#define NOMINMAX
#include "tchar.h"
#include "stdlib_win.h"

// static_assert(sizeof(long) == sizeof(int64_t));

char* _i64toa(int64_t _Value, char* _Buffer, int _Radix) {
    if (_Buffer == nullptr || _Radix < 2 || _Radix > 36) {
        return _Buffer;
    }
    if (_Radix == 10) {
        sprintf(_Buffer, "%ld", _Value);
        return _Buffer;
    }
    uint64_t t_val = *(uint64_t*)(&_Value);
    std::stack<uint8_t> vx;
    while (t_val != 0) {
        vx.push(t_val % _Radix);
        t_val = t_val / _Radix;
    }
    auto _t_buffer = _Buffer;
    while (vx.size() > 0) {
        if (vx.top() < 10) {
            *_t_buffer = '0' + vx.top();
        }
        else {
            *_t_buffer = 'a' + vx.top() - 10;
        }
        vx.pop();
        _t_buffer++;
    }
    *_t_buffer = '\0';
    return _Buffer;
}

char* _ui64toa(uint64_t _Value, char* _Buffer, int _Radix) {
    if (_Buffer == nullptr || _Radix < 2 || _Radix > 36) {
        return _Buffer;
    }
    if (_Radix == 10) {
        sprintf(_Buffer, "%lu", _Value);
        return _Buffer;
    }
    uint64_t t_val = *(uint64_t*)(&_Value);
    std::stack<uint8_t> vx;
    while (t_val != 0) {
        vx.push(t_val % _Radix);
        t_val = t_val / _Radix;
    }
    auto _t_buffer = _Buffer;
    while (vx.size() > 0) {
        if (vx.top() < 10) {
            *_t_buffer = '0' + vx.top();
        }
        else {
            *_t_buffer = 'a' + vx.top() - 10;
        }
        vx.pop();
        _t_buffer++;
    }
    *_t_buffer = '\0';
    return _Buffer;
}

int mbstou16cs_s(size_t* pReturnValue, char16_t* wcstr, size_t sizeInWords,
    const char* mbstr, size_t count) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    int ret = 0;
    std::string src(mbstr);
    auto dst = cvt.from_bytes(src);
    auto len = u16csnlen(dst.data(), std::min(count, sizeInWords));
    if (len != (size_t)-1) {
        len = len + 1;
    }
    len = std::min(len, count);
    len = std::min(len, sizeInWords);
    u16csncpy(wcstr, dst.data(), len);
    ret = (int)len;
    if (pReturnValue) {
        *pReturnValue = len;
    }
    return 0;
}

int u16cstombs_s(size_t* pReturnValue, char* mbstr, size_t sizeInBytes,
    const char16_t* wcstr, size_t count) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    int ret = 0;
    std::u16string src(wcstr);
    auto dst = cvt.to_bytes(src);
    auto len = strnlen(dst.c_str(), std::min(count, sizeInBytes));
    if (len != (size_t)-1) {
        len = len + 1;
    }
    len = std::min(len, count);
    len = std::min(len, sizeInBytes);
    strncpy(mbstr, dst.c_str(), len);
    ret = (int)len;
    if (pReturnValue) {
        *pReturnValue = len;
    }
    return 0;
}

int mbstou16cs(char16_t* wcstr, const char* mbstr, size_t count) {
    size_t ret = 0;
    mbstou16cs_s(&ret, wcstr, count, mbstr, count);
    return ret;
}

int u16cstombs(char* mbstr, const char16_t* wcstr, size_t count) {
    size_t ret = 0;
    u16cstombs_s(&ret, mbstr, count, wcstr, count);
    return ret;
}

int u16cstoi(char16_t* wcstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    auto str = cvt.to_bytes(wcstr);
    return atoi(str.c_str());
}

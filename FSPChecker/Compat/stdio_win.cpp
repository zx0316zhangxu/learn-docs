#include <stdio_win.h>
#include <locale>
#include <codecvt>
#include <errno.h>

errno_t _wfopen_s(FILE** pFile, const char16_t *filename, const char16_t *mode) {
    if (!pFile || !filename || !mode) {
        return EINVAL;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    auto sfilename = cvt.to_bytes(filename);
    auto smode = cvt.to_bytes(mode);
    auto delim_pos = smode.find(",");
    if (delim_pos != std::string::npos) {
        smode = smode.substr(0, delim_pos);
    }
    *pFile = fopen(sfilename.c_str(), smode.c_str());
    if (!*pFile) {
        return EINVAL;
    }
    return 0;
}

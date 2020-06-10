#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <codecvt>
#include <locale>
#include <dlfcn.h>
#include <iostream>
#include <algorithm>
#define NOMINMAX
#include "common_handle.h"
#include "tchar.h"
#include "libloaderapi.h"
#include "winerror.h"

// void _wrapper_init() __attribute__((constructor, weak, used));
void _wrapper_init(DllMain_t dllmain) {
    try {
        BOOST_LOG_TRIVIAL(info)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << "Module Attaching..., symbol: " << (void*)dllmain;
        auto sModulePath
            = boost::filesystem::absolute(boost::dll::symbol_location(*dllmain))
                               .string();
        auto nativeHandle = dlopen(sModulePath.c_str(), RTLD_NOW);
        auto handle = __compat_handle_reg_module(nativeHandle, sModulePath);
        BOOST_LOG_TRIVIAL(info)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << "Module Attach: " << sModulePath << ", Handle:" << handle
            << ", NativeHandle:" << nativeHandle;
        dllmain((HMODULE)handle, DLL_PROCESS_ATTACH, 0);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
}

// void _wrapper_fini() __attribute__((destructor, weak, used));
void _wrapper_fini(DllMain_t dllmain) {
    try {
        BOOST_LOG_TRIVIAL(info)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << "Module Detaching..., symbol: " << (void*)dllmain;
        auto sModulePath
            = boost::filesystem::absolute(boost::dll::symbol_location(*dllmain))
                               .string();
        auto nativeHandle = dlopen(sModulePath.c_str(), RTLD_NOW);
        auto handle
            = __compat_handle_find_module_by_native_handle(nativeHandle);
        __compat_handle_erase(handle);
        BOOST_LOG_TRIVIAL(info)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << "Module Detach: " << sModulePath << ", Handle:" << handle
            << ", NativeHandle:" << nativeHandle;
        dllmain((HMODULE)handle, DLL_PROCESS_DETACH, 0);
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
}

DWORD
WINAPI
GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize) {
    auto ret = 0;
    if (hModule != (HMODULE)-1) {
        try {
            char fileName[UINT8_MAX];
            auto ret = GetModuleFileNameA(hModule, fileName, sizeof(fileName));
            if (ret > 0) {
                if (lpFilename != nullptr) {
                    std::string sFileName(fileName, ret);
                    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,
                        char16_t>
                        cvt;
                    auto wsFileName = cvt.from_bytes(sFileName);
                    auto nCopy
                        = std::min((size_t)nSize, wsFileName.length() + 1);
                    u16csncpy(lpFilename, wsFileName.data(), nCopy);
                    ret = nCopy;
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

DWORD
WINAPI
GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
    auto ret = 0;
    if (hModule != (HMODULE)-1) {
        try {
            if (lpFilename != nullptr) {
                auto fullPath
                    = __compat_handle_get_module_full_path((int)hModule);
                auto moduleFileName
                    = boost::filesystem::path(fullPath).string();
                auto szCopy
                    = std::min((size_t)nSize, moduleFileName.length() + 1);
                realpath(moduleFileName.c_str(), lpFilename);
                ret = szCopy;
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

HINSTANCE LoadLibraryW(const char16_t* path) {
    auto ret = (HINSTANCE)-1;
    if (path != nullptr) {
        try {
            auto wsPath = std::u16string(path);
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            ret = LoadLibraryA(cvt.to_bytes(wsPath).c_str());
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

HINSTANCE LoadLibraryA(const char* path) {
    auto ret = (HINSTANCE)-1;
    if (path != nullptr) {
        try {
            auto nativeHandle = dlopen(path, RTLD_NOW);
            if (nativeHandle != 0) {
                ret = (HINSTANCE)__compat_handle_reg_module(
                    nativeHandle, std::string(path));
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

void* GetProcAddressW(HINSTANCE i, const char16_t* symbol) {
    void* ret = nullptr;
    if (symbol != nullptr) {
        try {
            auto wsSymbol = std::u16string(symbol);
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            ret = GetProcAddressA(i, cvt.to_bytes(wsSymbol).c_str());
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

void* GetProcAddressA(HINSTANCE i, const char* symbol) {
    void* ret = nullptr;
    if (symbol != nullptr) {
        try {
            auto sSymbol = std::string(symbol);
            auto nativeHandle
                = __compat_handle_get_module_native_handle((int)i);
            if (nativeHandle != 0) {
                ret = dlsym(nativeHandle, sSymbol.c_str());
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}

BOOL FreeLibrary(HINSTANCE i) {
    if (i != (HINSTANCE)-1) {
        try {
            auto nativeHandle
                = __compat_handle_get_module_native_handle((int)i);
            dlclose(nativeHandle);
            __compat_handle_erase((int)i);
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }

    return TRUE;
}

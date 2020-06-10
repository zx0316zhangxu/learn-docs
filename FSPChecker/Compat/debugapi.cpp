#include <codecvt>
#include <locale>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "debugapi.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

// https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw
VOID OutputDebugStringW(LPCWSTR lpOutputString) {
    if (lpOutputString == nullptr) {
        BOOST_LOG_TRIVIAL(warning)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << u8"OutputDebugStringW: lpOutputString is null";
        return;
    }
    else {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
        BOOST_LOG_TRIVIAL(debug)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " :"
            << cvt.to_bytes(lpOutputString);
    }
}

VOID OutputDebugStringA(LPCSTR lpOutputString) {
    if (lpOutputString == nullptr) {
        BOOST_LOG_TRIVIAL(warning)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << u8"OutputDebugStringW: lpOutputString is null";
        return;
    }
    else {
        BOOST_LOG_TRIVIAL(debug) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " :" << lpOutputString;
    }
}

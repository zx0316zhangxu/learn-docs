#include <boost/filesystem.hpp>
#include "direct.h"

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/mkdir-wmkdir
int _mkdir(const char* dirname) {
    if (dirname == nullptr) {
        return -1;
    }
    if (boost::filesystem::exists(dirname)) {
        return -1;
    }
    else {
        if (boost::filesystem::create_directories(dirname)) {
            return 0;
        }
        else {
            return -1;
        }
    }
}

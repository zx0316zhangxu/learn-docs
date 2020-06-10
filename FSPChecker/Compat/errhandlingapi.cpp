#include <boost/log/trivial.hpp>
#include "errhandlingapi.h"

// TODO: not implemented
DWORD GetLastError() {
    BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ##TODO##" ;

    return 0;
}

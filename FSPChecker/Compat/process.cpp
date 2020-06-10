#include <mutex>
#include <thread>
#include <pthread.h>
#include <boost/log/trivial.hpp>
#include "common_handle.h"
#include "process.h"

void _endthread(void) {
	auto tid = pthread_self();
	pthread_cancel(tid);
    BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid;
}

uintptr_t _beginthread(
    void (*start_address)(void*), unsigned stack_size, void* arglist) {
    uintptr_t handle = 0;
    if (start_address != nullptr) {
        try {
            auto pt = new std::thread(start_address, arglist);
            handle = __compat_handle_reg_thread(pt->native_handle());
            BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)pt->native_handle();
            pt->detach();
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }



    return handle;
}

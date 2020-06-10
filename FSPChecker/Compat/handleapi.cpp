#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <boost/log/trivial.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include "common_handle.h"
#include "handleapi.h"
#include "eventapi.h"

BOOL CloseHandle(HANDLE hObject) {
    auto ret = FALSE;
    auto type = __compat_handle_type_get((int)hObject);
    int fd = -1;
    void* hmodule = nullptr;
    pthread_t tid = 0;
    std::string mutexName = "";
    std::string eventName = "";
    try {
        switch (type) {
        case __compat_handle_type_t::FILE:
            fd = __compat_handle_get_fd((int)hObject);
            try {
                if (fd != -1) {
                    close(fd);
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
            break;

        case __compat_handle_type_t::MODULE:
            hmodule = __compat_handle_get_module_native_handle((int)hObject);
            try {
                if (hmodule != 0) {
                    dlclose(hmodule);
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
            break;

        case __compat_handle_type_t::THREAD:
            tid = __compat_handle_get_thread_tid((int)hObject);
            try {
                if (tid != 0) {
                    //pthread_cancel(tid);
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
            break;

        case __compat_handle_type_t::MUTEX:
#if !USE_BOOST_MUTEX
            mutexName = __compat_handle_mutex_get_name((int)hObject);
            try {
                auto mtx = (pthread_mutex_t*)__compat_handle_mutex_get((int)hObject);
                auto attr = (pthread_mutexattr_t*)__compat_handle_mutex_get_attr((int)hObject);
                if (mutexName != "") {
                    // not use shared sem for debug
                    pthread_mutex_destroy( mtx );
                    delete mtx;
                    delete attr;
                    ret = TRUE;
                }else{
                    pthread_mutex_destroy( mtx );
                    delete mtx;
                    delete attr;
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
#else

            mutexName = __compat_handle_get_mutex_name((int)hObject);
            try {
                if (mutexName != "") {
                    boost::interprocess::named_recursive_mutex::remove(
                        mutexName.c_str());
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
#endif
            break;

        case __compat_handle_type_t::EVENT:
#if !USE_BOOST_EVENT
            try {
                auto hEvent= (event_handle)__compat_handle_get_event((int)hObject);
                if (hEvent != NULL) {
                	event_destroy(hEvent);
                    ret = TRUE;
                }
                __compat_handle_erase((int)hObject);
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
#else
            eventName = __compat_handle_get_event_name((int)hObject);
            if (eventName != "") {
                boost::interprocess::named_condition::remove(
                    eventName.c_str());
                boost::interprocess::named_mutex::remove(
                    (eventName + "_mtx").c_str());
            }

#endif

            break;

        default:
            break;
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return ret;
}

#include <boost/log/trivial.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <locale>
#include <codecvt>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include "common_handle.h"
#include "synchapi.h"

#include "eventapi.h"

// #define SYNC_TRACE
// #define MUTEX_LOCK_DEBUG 1
#define MUTEX_LOCK_ENABLED 1
// #define EVENT_WAIT_DEBUG 1
#define EVENT_WAIT_ENABLED 1

#define INFINITE (DWORD) - 1

HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner,
    LPCWSTR lpName) {
    auto handle = (HANDLE)NULL;
    try {
        if (lpName != nullptr) {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            handle = CreateMutexA(lpMutexAttributes, bInitialOwner,
                cvt.to_bytes(std::u16string(lpName)).c_str());
        }
        else {
            handle = CreateMutexA(lpMutexAttributes, bInitialOwner, NULL);
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return handle;
}

HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner,
    LPCSTR lpName) {

#if !USE_BOOST_MUTEX
    auto handle = (HANDLE)NULL;
    pthread_mutex_t *mtx = (pthread_mutex_t*)NULL;
    pthread_mutexattr_t *mtxattr = (pthread_mutexattr_t*)NULL;

    std::string mutexName = "";
    if (lpName != nullptr) {
        mutexName = std::string(lpName);
        handle = (HANDLE)__compat_handle_mutex_find_by_name(mutexName);
    }
    
    if(handle == NULL) {
        pthread_mutex_t *mtx = new pthread_mutex_t();
        pthread_mutexattr_t *mtxattr = new pthread_mutexattr_t();

        pthread_mutexattr_settype(mtxattr, PTHREAD_MUTEX_RECURSIVE_NP);

        if( 0 == pthread_mutex_init(mtx,mtxattr) ){
            handle = (HANDLE)__compat_handle_mutex_reg(mtx, mtxattr, mutexName);
        }else{
            delete mtx;
            delete mtxattr;
        }
    }

    if(handle != NULL){
        if(bInitialOwner == TRUE){
            mtx = (pthread_mutex_t *)__compat_handle_mutex_get((int)handle);
            pthread_mutex_lock( mtx );
        }
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " CreateMutexA Failed ";
    }

    return handle;

#else
    auto handle = (HANDLE)NULL;
    try {
        std::string mutexName;
        if (lpName != nullptr) {
            mutexName = std::string(lpName);
        }
        else {
            auto uuid = boost::uuids::random_generator()();
            mutexName = boost::uuids::to_string(uuid);
        }
#ifdef SYNC_TRACE
        BOOST_LOG_TRIVIAL(trace)
            << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
            << "Creating Mutex: " << mutexName
            << " bInitialOwner:" << bInitialOwner;
#endif
        using namespace boost::interprocess;
        bool created = false;
        named_recursive_mutex* pMtx = nullptr;
        try {
            pMtx = new named_recursive_mutex(open_only, mutexName.c_str());
        }
        catch (const std::exception& e) {
            // do nothing
            pMtx = nullptr;
        }

        if (!pMtx) {
            pMtx = new named_recursive_mutex(open_or_create, mutexName.c_str());
            created = true;
        }
        if (pMtx) {
            try {
                if (created) {
                    handle = (HANDLE)__compat_handle_reg_mutex(mutexName);
#ifdef SYNC_TRACE
                    BOOST_LOG_TRIVIAL(trace)
                        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__
                        << " "
                        << "Created Mutex: " << mutexName << " "
                        << bInitialOwner << " " << handle;
#endif
                }
                else {
                    handle = (HANDLE)__compat_handle_find_mutex_by_name;
#ifdef SYNC_TRACE
                    BOOST_LOG_TRIVIAL(trace)
                        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__
                        << " "
                        << "Created Mutex (Opened): " << mutexName << " "
                        << bInitialOwner << " " << handle;
#endif
                }
            }
            catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__
                                         << " " << __LINE__ << " " << e.what();
            }
            if (bInitialOwner) {
#ifdef MUTEX_LOCK_ENABLED
#if 0
            try {
                    int retry = 10;
                    while (!pMtx->try_lock() && retry > 0) {
                        retry--;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                    if (retry > 0) {
                        // do nothing
                }
                else {
                        delete pMtx;
                        named_recursive_mutex::remove(mutexName.c_str());
                        pMtx = new named_recursive_mutex(
                            open_or_create, mutexName.c_str());
                        created = true;
                        BOOST_LOG_TRIVIAL(error)
                            << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " "
                            << "initial lock failed, recreate mutex";
                        pMtx->lock();
                }
            }
            catch (const std::exception& e) {
                    BOOST_LOG_TRIVIAL(error)
                        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__
                        << " " << e.what();
                }
#else
                pMtx->lock();
#endif
#endif
            }
            delete pMtx;
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return handle;
#endif
}

BOOL ReleaseMutex(HANDLE hMutex) {
#if !USE_BOOST_MUTEX
    auto ret = FALSE;

    if( NULL == hMutex ){
        return FALSE;
    }
    auto mtx = (pthread_mutex_t *)__compat_handle_mutex_get((int)hMutex);
    if( NULL == mtx ){
        return FALSE;
    }
    if( 0 == pthread_mutex_trylock( mtx ) ){
        pthread_mutex_unlock( mtx );
        pthread_mutex_unlock( mtx );
        ret = TRUE;
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                    << __LINE__ << " ReleaseMutex Failed ";
    }
    return ret;
#else
    auto ret = FALSE;
    if (hMutex != NULL) {
        try {
            auto mutexName = __compat_handle_get_mutex_name((int)hMutex);
#ifdef SYNC_TRACE
            BOOST_LOG_TRIVIAL(trace)
                << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                << "Releasing Mutex: " << mutexName << " " << hMutex;
#endif
            if (mutexName != "") {
                using namespace boost::interprocess;
                auto pMtx
                    = new named_recursive_mutex(open_only, mutexName.c_str());
                if (pMtx) {
#if 1
                    pMtx->unlock();
#else
#ifdef MUTEX_LOCK_ENABLED
                    // check b owner
                    if (pMtx->try_lock()) {
                        pMtx->unlock();
                        pMtx->unlock();
                        ret = TRUE;
#ifdef SYNC_TRACE
                        BOOST_LOG_TRIVIAL(trace)
                            << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " "
                            << "Released Mutex: " << mutexName << " " << hMutex;
#endif
                    }
                    else {
#ifdef SYNC_TRACE
                        BOOST_LOG_TRIVIAL(warning)
                            << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " "
                            << "Tried to release foreign Mutex: " << mutexName
                            << " " << hMutex;
#endif
                    }
#endif

#endif
                    delete pMtx;
                }
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
#endif
}

DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    // only used on mutex and event
    auto ret = WAIT_FAILED;
    auto tstart = std::chrono::high_resolution_clock::now();
    if (hHandle != NULL) {
        try {
            __compat_handle_type_t objectType = __compat_handle_type_get((int)hHandle);
            std::string mutexName = "";
            std::string eventName = "";
            auto mtx = (pthread_mutex_t *)NULL;
            auto event = (event_handle)NULL;

            switch (objectType) {
            case __compat_handle_type_t::MUTEX:
#if !USE_BOOST_MUTEX
                mtx = (pthread_mutex_t *)__compat_handle_mutex_get((int)hHandle);
                if (dwMilliseconds != INFINITE) {
                    struct timespec abstime;
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    abstime.tv_sec  = tv.tv_sec + dwMilliseconds / 1000;
                    abstime.tv_nsec = tv.tv_usec*1000 + (dwMilliseconds % 1000)*1000000;
                    if (abstime.tv_nsec >= 1000000000)
                    {
                        abstime.tv_nsec -= 1000000000;
                        abstime.tv_sec++;
                    }

                    if( 0 == pthread_mutex_timedlock(mtx, &abstime) ){
                        ret = WAIT_OBJECT_0;
                    }else{
                        ret = WAIT_TIMEOUT;
                    }

                }else{
                    if(0==pthread_mutex_lock(mtx)){
                        ret = WAIT_OBJECT_0;
                    }else{
                        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                                    << __LINE__ << " sem_wait Failed ";
                        ret = WAIT_TIMEOUT;
                    }
                }
                break;
#else
#ifdef MUTEX_LOCK_DEBUG
                if (dwMilliseconds == INFINITE) {
                    dwMilliseconds = 30 * 86400 * 1000;
                }
#endif
                mutexName = __compat_handle_get_mutex_name((int)hHandle);
                if (mutexName != "") {
#ifdef SYNC_TRACE
                    BOOST_LOG_TRIVIAL(trace)
                        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__
                        << " "
                        << "Waiting for Mutex: " << mutexName << " " << hHandle
                        << " " << dwMilliseconds;
#endif
                    using namespace boost::interprocess;
                    auto pMtx = new named_recursive_mutex(
                        open_only, mutexName.c_str());
                    if (pMtx) {
                        bool succeed = false;
#if 1
                        if (dwMilliseconds != INFINITE) {
                            succeed = pMtx->timed_lock(boost::get_system_time() + 
                                            boost::posix_time::milliseconds(dwMilliseconds));
                        }else{
                            pMtx->lock();
                            succeed = true;
                        }
#else
                        if (dwMilliseconds != INFINITE) {
                            while (std::chrono::duration_cast<
                                       std::chrono::milliseconds>(
                                       std::chrono::high_resolution_clock::now()
                                       - tstart)
                                       .count()
                                <= dwMilliseconds) {
#ifdef MUTEX_LOCK_ENABLED
                                succeed = pMtx->try_lock();
#else
                                succeed = true;
#endif
                                if (succeed) {
                                    break;
                                }
                                else {
                                    std::this_thread::sleep_for(
                                        std::chrono::milliseconds(1));
                                }
                                if (std::chrono::duration_cast<
                                        std::chrono::milliseconds>(
                                        std::chrono::high_resolution_clock::
                                            now()
                                        - tstart)
                                        .count()
                                    > 3 * 1000) {
                                    BOOST_LOG_TRIVIAL(warning)
                                        << __FILE__ << " " << __FUNCTION__
                                        << " " << __LINE__ << " "
                                        << "Waited Mutex too long: "
                                        << mutexName << " " << hHandle << " "
                                        << std::chrono::duration_cast<
                                               std::chrono::milliseconds>(
                                               std::chrono::
                                                   high_resolution_clock::now()
                                               - tstart)
                                               .count();
                                    std::this_thread::sleep_for(
                                        std::chrono::seconds(1));
#ifdef MUTEX_LOCK_DEBUG
                                    ret = true;
                                    break;
#endif
                                }
                            }
                        }
                        else {
#ifdef MUTEX_LOCK_ENABLED
                            while (!pMtx->try_lock()) {
                                if (std::chrono::duration_cast<
                                        std::chrono::milliseconds>(
                                        std::chrono::high_resolution_clock::
                                            now()
                                        - tstart)
                                        .count()
                                    > 3 * 1000) {
                                    BOOST_LOG_TRIVIAL(warning)
                                        << __FILE__ << " " << __FUNCTION__
                                        << " " << __LINE__ << " "
                                        << "Waited Mutex too long: "
                                        << mutexName << " " << hHandle << " "
                                        << std::chrono::duration_cast<
                                               std::chrono::milliseconds>(
                                               std::chrono::
                                                   high_resolution_clock::now()
                                               - tstart)
                                               .count();
                                    std::this_thread::sleep_for(
                                        std::chrono::seconds(1));
                                }
                                std::this_thread::sleep_for(
                                    std::chrono::milliseconds(1));
                            }
#endif
                            succeed = true;
                        }

#endif

                        if (succeed) {
                            // successfully locked
                            ret = WAIT_OBJECT_0;
#ifdef SYNC_TRACE
                            BOOST_LOG_TRIVIAL(trace)
                                << __FILE__ << " " << __FUNCTION__ << " "
                                << __LINE__ << " "
                                << "Wait for mutex succeed: " << mutexName
                                << " " << hHandle;
#endif
                        }
                        else {
                            // timeout
                            ret = WAIT_TIMEOUT;
#ifdef SYNC_TRACE
                            BOOST_LOG_TRIVIAL(trace)
                                << __FILE__ << " " << __FUNCTION__ << " "
                                << __LINE__ << " "
                                << "Wait for mutex failed: " << mutexName << " "
                                << hHandle;
#endif
                        }
                        delete pMtx;
                    }
                }
                break;
#endif
            case __compat_handle_type_t::EVENT:
#if !USE_BOOST_EVENT
            	event = (event_handle)__compat_handle_get_event((int)hHandle);
            	if(event != NULL){
                	if (dwMilliseconds == INFINITE) {
                		if(-1==event_wait(event)){
                			ret = WAIT_TIMEOUT;
                		}else{
                			ret = WAIT_OBJECT_0;
                		}
                	}else{
                		if(1==event_timedwait(event, dwMilliseconds)){
                			ret = WAIT_TIMEOUT;
                		}else{
                			ret = WAIT_OBJECT_0;
                		}

                	}
            	}else{
            		BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
            		                                 << __LINE__ << " : WaitForSingleObject Failed";
            	}
                break;

#else
#ifdef EVENT_WAIT_DEBUG
                if (dwMilliseconds == INFINITE) {
                    dwMilliseconds = 30 * 86400 * 1000;
                }
#endif
                eventName = __compat_handle_get_event_name((int)hHandle);
                if (eventName != "") {
                    using namespace boost::interprocess;
                    auto pEvent
                        = new named_condition(open_only, eventName.c_str());
                    auto pEventMutex = new named_mutex(
                        open_only, (eventName + "_mtx").c_str());
                    if (pEvent && pEventMutex) {
                        if (__compat_handle_get_event_status((int)hHandle)) {
                            ret = WAIT_OBJECT_0;
                        }
                        else {
                            bool succeed = false;
                            if (dwMilliseconds != INFINITE) {
                                while (std::chrono::duration_cast<
                                           std::chrono::milliseconds>(
                                           std::chrono::high_resolution_clock::
                                               now()
                                           - tstart)
                                           .count()
                                    <= dwMilliseconds) {
                                    scoped_lock<named_mutex> lck(
                                        *pEventMutex, defer_lock);
#ifdef EVENT_WAIT_ENABLED
                                    if (lck.try_lock()) {
                                        succeed = pEvent->timed_wait(lck,
                                            boost::get_system_time()
                                                + boost::posix_time::
                                                    milliseconds(
                                                        dwMilliseconds < 1000
                                                            ? dwMilliseconds
                                                            : 1000));
                                        lck.unlock();
                                    }
#else
                                    succeed = true;
#endif
                                    if (succeed) {
                                        break;
                                    }
                                    else {
                                        std::this_thread::sleep_for(
                                            std::chrono::milliseconds(1));
                                    }
                                    if (std::chrono::duration_cast<
                                            std::chrono::milliseconds>(
                                            std::chrono::high_resolution_clock::
                                                now()
                                            - tstart)
                                            .count()
                                        > 3 * 1000) {
                                        BOOST_LOG_TRIVIAL(warning)
                                            << __FILE__ << " " << __FUNCTION__
                                            << " " << __LINE__ << " "
                                            << "Waited Event too long: "
                                            << eventName << " " << hHandle
                                            << " "
                                            << std::chrono::duration_cast<
                                                   std::chrono::milliseconds>(
                                                   std::chrono::
                                                       high_resolution_clock::
                                                           now()
                                                   - tstart)
                                                   .count();
#ifdef EVENT_WAIT_DEBUG
                                        succeed = true;
                                        break;
#endif
                                        std::this_thread::sleep_for(
                                            std::chrono::seconds(1));
                                    }
                                }
                            }
                            else {
                                scoped_lock<named_mutex> lck(
                                    *pEventMutex, defer_lock);
                                while (true) {
                                    if (std::chrono::duration_cast<
                                            std::chrono::milliseconds>(
                                            std::chrono::high_resolution_clock::
                                                now()
                                            - tstart)
                                            .count()
                                        > 3 * 1000) {
                                        BOOST_LOG_TRIVIAL(warning)
                                            << __FILE__ << " " << __FUNCTION__
                                            << " " << __LINE__ << " "
                                            << "Waited Event too long: "
                                            << eventName << " " << hHandle
                                            << " "
                                            << std::chrono::duration_cast<
                                                   std::chrono::milliseconds>(
                                                   std::chrono::
                                                       high_resolution_clock::
                                                           now()
                                                   - tstart)
                                                   .count();
                                        std::this_thread::sleep_for(
                                            std::chrono::seconds(1));
                                    }
#ifdef EVENT_WAIT_ENABLED
                                    // if (lck.try_lock()) {
                                    //     succeed = pEvent->timed_wait(lck,
                                    //         boost::get_system_time()
                                    //             + boost::posix_time::seconds(
                                    //                 1));
                                    //     lck.unlock();
                                    // }

                                    if (lck.timed_lock(boost::get_system_time()
                                            + boost::posix_time::seconds(30))) {
                                        succeed = pEvent->timed_wait(lck,
                                            boost::get_system_time()
                                                + boost::posix_time::seconds(
                                                    30));
                                        lck.unlock();
                                    }
                                    // succeed = true;
#else
                                    succeed = true;
#endif
                                    if (!succeed) {
                                        std::this_thread::sleep_for(
                                            std::chrono::milliseconds(1));
                                    }
                                    else {
                                        break;
                                    }
                                }
                            }
                            if (succeed) {
                                // successfully locked
                                ret = WAIT_OBJECT_0;
                            }
                            else {
                                // timeout
                                ret = WAIT_TIMEOUT;
                            }
                        }
                        delete pEventMutex;
                        delete pEvent;
                    }
                }
                break;
#endif

            default:
                break;
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
}
#if 0 // not support
DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles,
    BOOL bWaitAll, DWORD dwMilliseconds) {
    // only used on event
    auto ret = WAIT_FAILED;
    while(1){
        std::this_thread::sleep_for(
            std::chrono::seconds(100));    
    }
#if 0
    auto tstart = std::chrono::high_resolution_clock::now();
    if (nCount > 0 && lpHandles != NULL) {
        try {
#ifdef EVENT_WAIT_DEBUG
            if (dwMilliseconds == INFINITE) {
                dwMilliseconds = 30 * 86400 * 1000;
            }
#endif
            std::atomic_int32_t wDone = 0;
            for (int i = 0; i < nCount; i++) {
                std::thread([&] {
                    std::string mutexName = "";
                    std::string eventName = "";
                    eventName
                        = __compat_handle_get_event_name((int)lpHandles[i]);
                    if (eventName != "") {
                        using namespace boost::interprocess;
                        auto pEvent
                            = new named_condition(open_only, eventName.c_str());
                        auto pEventMutex = new named_mutex(
                            open_only, (eventName + "_mtx").c_str());
                        if (pEvent && pEventMutex) {
                            if (__compat_handle_get_event_status(
                                    (int)lpHandles[i])) {
                                wDone.fetch_add(1);
                            }
                            else {
                                bool succeed = false;
                                if (dwMilliseconds != INFINITE) {
                                    while (std::chrono::duration_cast<
                                               std::chrono::milliseconds>(
                                               std::chrono::
                                                   high_resolution_clock::now()
                                               - tstart)
                                               .count()
                                        <= dwMilliseconds) {
                                        if (bWaitAll == FALSE && wDone > 0) {
                                            break;
                                        }
                                        scoped_lock<named_mutex> lck(
                                            *pEventMutex, defer_lock);
#ifdef EVENT_WAIT_ENABLED
                                        if (lck.try_lock()) {
                                            succeed = pEvent->timed_wait(lck,
                                                boost::get_system_time()
                                                    + boost::posix_time::
                                                        milliseconds(
                                                            dwMilliseconds
                                                                    < 1000
                                                                ? dwMilliseconds
                                                                : 1000));
                                            lck.unlock();
                                        }
#else
                                        succeed = true;
#endif
                                        if (succeed) {
                                            break;
                                        }
                                        else {
                                            std::this_thread::sleep_for(
                                                std::chrono::milliseconds(1));
                                        }
                                        if (std::chrono::duration_cast<
                                                std::chrono::milliseconds>(
                                                std::chrono::
                                                    high_resolution_clock::now()
                                                - tstart)
                                                .count()
                                            > 3 * 1000) {
                                            BOOST_LOG_TRIVIAL(warning)
                                                << __FILE__ << " "
                                                << __FUNCTION__ << " "
                                                << __LINE__ << " "
                                                << "Waited Event too long: "
                                                << eventName << " "
                                                << lpHandles[i] << " "
                                                << std::chrono::duration_cast<
                                                       std::chrono::
                                                           milliseconds>(
                                                       std::chrono::
                                                           high_resolution_clock::
                                                               now()
                                                       - tstart)
                                                       .count();
#ifdef EVENT_WAIT_DEBUG
                                            succeed = true;
                                            break;
#endif
                                            std::this_thread::sleep_for(
                                                std::chrono::seconds(1));
                                        }
                                    }
                                }
                                else {
                                    scoped_lock<named_mutex> lck(
                                        *pEventMutex, defer_lock);
                                    while (true) {
                                        if (bWaitAll == FALSE && wDone > 0) {
                                            break;
                                        }
                                        if (std::chrono::duration_cast<
                                                std::chrono::milliseconds>(
                                                std::chrono::
                                                    high_resolution_clock::now()
                                                - tstart)
                                                .count()
                                            > 3 * 1000) {
                                            BOOST_LOG_TRIVIAL(warning)
                                                << __FILE__ << " "
                                                << __FUNCTION__ << " "
                                                << __LINE__ << " "
                                                << "Waited Event too long: "
                                                << eventName << " "
                                                << lpHandles[i] << " "
                                                << std::chrono::duration_cast<
                                                       std::chrono::
                                                           milliseconds>(
                                                       std::chrono::
                                                           high_resolution_clock::
                                                               now()
                                                       - tstart)
                                                       .count();
                                            std::this_thread::sleep_for(
                                                std::chrono::seconds(1));
                                        }
#ifdef EVENT_WAIT_ENABLED
                                        if (lck.try_lock()) {
                                            succeed = pEvent->timed_wait(lck,
                                                boost::get_system_time()
                                                    + boost::posix_time::
                                                        seconds(1));
                                            lck.unlock();
                                        }
#else
                                        succeed = true;
#endif
                                        if (!succeed) {
                                            std::this_thread::sleep_for(
                                                std::chrono::milliseconds(1));
                                        }
                                        else {
                                            break;
                                        }
                                    }
                                }
                                if (succeed) {
                                    // successfully locked
                                    wDone.fetch_add(1);
                                }
                                else {
                                    // timeout
                                }
                            }
                            delete pEventMutex;
                            delete pEvent;
                        }
                    }
                });
            }
            if (bWaitAll == TRUE && wDone == nCount) {
                ret = WAIT_OBJECT_0;
            }
            else if (bWaitAll == TRUE && wDone != nCount) {
                ret = WAIT_TIMEOUT;
            }
            else if (bWaitAll == FALSE && wDone > 0) {
                ret = WAIT_OBJECT_0;
            }
            if (bWaitAll == FALSE && wDone == 0) {
                ret = WAIT_TIMEOUT;
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
#endif
    return ret;
}
#endif

HANDLE CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
    BOOL bInitialState, LPCWSTR lpName) {
    HANDLE handle = NULL;
    try {
        if (lpName != nullptr) {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
                cvt;
            handle = CreateEventA(lpEventAttributes, bManualReset,
                bInitialState, cvt.to_bytes(std::u16string(lpName)).c_str());
        }
        else {
            handle = CreateEventA(
                lpEventAttributes, bManualReset, bInitialState, NULL);
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return handle;
}

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
    BOOL bInitialState, LPCSTR lpName) {
#if !USE_BOOST_EVENT
	event_handle hEvent = NULL;
	HANDLE handle = NULL;
	try{
		hEvent = event_create(bManualReset, bInitialState);
		if(hEvent!=NULL){
            handle = (HANDLE)__compat_handle_reg_event(
            		(void*)hEvent, bManualReset, bInitialState);
		}else{
	        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
	                                 << __LINE__ << " : NULL";
		}
	}catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << "; handle"<< handle << "; e=" << e.what();
    }
	return handle;
#else
    auto handle = (HANDLE)NULL;
    try {
        std::string eventName;
        if (lpName != nullptr) {
            eventName = std::string(lpName);
        }
        else {
            auto uuid = boost::uuids::random_generator()();
            eventName = boost::uuids::to_string(uuid);
        }
        using namespace boost::interprocess;
        bool created = false;
        named_condition* pEvent = nullptr;
        try {
            pEvent = new named_condition(open_only, eventName.c_str());
        }
        catch (const std::exception&) {
            // do nothing
            pEvent = nullptr;
        }
        if (!pEvent) {
            pEvent = new named_condition(open_or_create, eventName.c_str());
            created = true;
        }
        named_mutex* pEventMutex = nullptr;
        try {
            pEventMutex
                = new named_mutex(open_only, (eventName + "_mtx").c_str());
        }
        catch (const std::exception&) {
            pEventMutex = nullptr;
        }
        if (!pEventMutex) {
            pEventMutex
                = new named_mutex(open_or_create, (eventName + "_mtx").c_str());
        }
        if (pEvent && pEventMutex) {
            if (created) {
                handle = (HANDLE)__compat_handle_reg_event(
                    eventName, bManualReset, bManualReset && bInitialState);
#ifdef SYNC_TRACE
                BOOST_LOG_TRIVIAL(trace)
                    << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                    << "Created Event: " << eventName << " " << bManualReset
                    << " " << bInitialState << " " << handle;
#endif
            }
            else {
                handle = (HANDLE)__compat_handle_find_event_by_name(eventName);
#ifdef SYNC_TRACE
                BOOST_LOG_TRIVIAL(trace)
                    << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                    << "Created Event (Opened): " << eventName << " "
                    << bManualReset << " " << bInitialState << " " << handle;
#endif
            }
            if (bInitialState == TRUE) {
                if (bManualReset) {
                    pEvent->notify_all();
                }
                else {
                    pEvent->notify_one();
                }
            }
            delete pEventMutex;
            delete pEvent;
        }
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " " << e.what();
    }
    return handle;
#endif
}

BOOL SetEvent(HANDLE hEvent) {
#if !USE_BOOST_EVENT
	BOOL ret = FALSE;
	event_handle event = (event_handle)__compat_handle_get_event((int)hEvent);

	if(event != NULL){
		if(-1==event_set(event)){
			ret = FALSE;
		}else{
			ret = TRUE;
		}
	}else{

		BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
		                                 << __LINE__ << " : SetEvent Failed";
	}
	return ret;
#else
    BOOL ret = FALSE;
    if (hEvent != NULL) {
        try {
            auto eventName = __compat_handle_get_event_name((int)hEvent);
            using namespace boost::interprocess;
            auto pEvent = new named_condition(open_only, eventName.c_str());
            auto pEventMutex
                = new named_mutex(open_only, (eventName + "_mtx").c_str());
            if (pEvent && pEventMutex) {
                auto bManualReset
                    = __compat_handle_get_event_b_manual_reset((int)hEvent);
                if (bManualReset) {
                    __compat_handle_set_event_status((int)hEvent, true);
                    pEvent->notify_all();
                }
                else {
                    pEvent->notify_one();
                }
                ret = TRUE;
                delete pEventMutex;
                delete pEvent;
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
#endif
}

BOOL ResetEvent(HANDLE hEvent) {
#if !USE_BOOST_EVENT
	BOOL ret = FALSE;
	event_handle event = (event_handle)__compat_handle_get_event((int)hEvent);
	if(event != NULL){
		if(-1==event_reset(event)){
			ret = FALSE;
		}else{
			ret = TRUE;
		}
	}else{
		BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
		                                 << __LINE__ << " : ResetEvent Failed";
	}
	return ret;
#else
    BOOL ret = FALSE;
    if (hEvent != NULL) {
        try {
            auto eventName = __compat_handle_get_event_name((int)hEvent);
            using namespace boost::interprocess;
            auto pEvent = new named_condition(open_only, eventName.c_str());
            auto pEventMutex
                = new named_mutex(open_only, (eventName + "_mtx").c_str());
            if (pEvent && pEventMutex) {
                __compat_handle_set_event_status((int)hEvent, false);
                ret = TRUE;
                delete pEventMutex;
                delete pEvent;
            }
        }
        catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " " << e.what();
        }
    }
    return ret;
#endif
}

void Sleep(DWORD dwMilliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(dwMilliseconds));
}

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {

	pthread_mutex_t *mtx = new pthread_mutex_t();
	pthread_mutexattr_t *mtxattr = new pthread_mutexattr_t();

	pthread_mutexattr_settype(mtxattr, PTHREAD_MUTEX_RECURSIVE_NP);

	pthread_mutex_init(mtx,mtxattr);

#ifdef SYNC_TRACE
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "lpCriticalSection: " << (void*)lpCriticalSection << "; mtx=" << (void*)mtx;
#endif

	lpCriticalSection->LockSemaphore = (HANDLE)mtx;
	lpCriticalSection->SpinCount = (ULONG_PTR)mtxattr;
    lpCriticalSection->LockCount = -1;
}

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	pthread_mutex_t *mtx = (pthread_mutex_t *)lpCriticalSection->LockSemaphore;
	pthread_mutexattr_t *mtxattr = (pthread_mutexattr_t *)lpCriticalSection->SpinCount;

#ifdef SYNC_TRACE
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "lpCriticalSection: " << (void*)lpCriticalSection << "; mtx=" << (void*)mtx;
#endif

	pthread_mutex_destroy( mtx );
	delete mtx;
	delete mtxattr;

	lpCriticalSection->LockSemaphore = nullptr;
    lpCriticalSection->LockCount = -1;
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	pthread_mutex_t *mtx = (pthread_mutex_t *)lpCriticalSection->LockSemaphore;

#ifdef SYNC_TRACE
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "lpCriticalSection: " << (void*)lpCriticalSection << "; mtx=" << (void*)mtx;
#endif

    // lpCriticalSection->LockCount++;
	pthread_mutex_lock( mtx );
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
	pthread_mutex_t *mtx = (pthread_mutex_t *)lpCriticalSection->LockSemaphore;

#ifdef SYNC_TRACE
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "lpCriticalSection: " << (void*)lpCriticalSection << "; mtx=" << (void*)mtx;
#endif
	pthread_mutex_unlock( mtx );
    // lpCriticalSection->LockCount--;
}

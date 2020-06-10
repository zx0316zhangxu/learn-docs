#include <iostream>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include "common_handle.h"

#include <boost/log/trivial.hpp>
#include <chrono>
#include <thread>

#include <execinfo.h>

#if !USE_BOOST_MAP
static pthread_mutex_t lock_mutex = PTHREAD_MUTEX_INITIALIZER;
#define MAP_LOCK()      pthread_mutex_lock(&lock_mutex)
#define MAP_UNLOCK()    pthread_mutex_unlock(&lock_mutex)
#else
#define MAP_LOCK()      
#define MAP_UNLOCK()    
#endif

__compat_handle_map_t* __compat_handle_map_get() {

	auto tstart = std::chrono::high_resolution_clock::now();

#if !USE_BOOST_MAP
	static __compat_handle_map_t compat_map;
    
    return &compat_map;

#else
    static boost::interprocess::managed_shared_memory* pseg = nullptr;
    static __compat_handle_map_t* pmap = nullptr;
    if (pmap == nullptr) {
        using namespace boost::interprocess;
        pseg = new managed_shared_memory(open_or_create,
            __compat_handle_region_name, __compat_handle_region_size);
        auto segManager = pseg->get_segment_manager();
        pmap = pseg->find_or_construct<__compat_handle_map_t>(
            __compat_handle_map_name)(std::less<int>(), segManager);
        try {
            pmap->size();
        }
        catch (const std::exception&) {
            BOOST_LOG_TRIVIAL(error)
                << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
                << u8"corrupted shared memory, recreate it";
            shared_memory_object::remove(__compat_handle_region_name);
            pseg = new managed_shared_memory(open_or_create,
                __compat_handle_region_name, __compat_handle_region_size);
            auto segManager = pseg->get_segment_manager();
            pmap = pseg->find_or_construct<__compat_handle_map_t>(
                __compat_handle_map_name)(std::less<int>(), segManager);
        }
    }

//    count ++;
//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//
////		{
////			void *bt[20];
////			char **strings;
////			size_t sz;
////			sz = backtrace(bt, 20);
////			strings = backtrace_symbols(bt, sz);
////			for(int i = 0; i < sz; ++i)
////				printf("     >>>>>%s \n", strings[i]);
////
////		}
//    }

    return pmap;
#endif
}

int __compat_handle_new_key_get() {
    int key = std::rand();
    auto map = __compat_handle_map_get();

    while (map->find(key) != map->end()) {
        key = key + 1;
    }
    map->insert(std::make_pair<>(key, __compat_handle_header_t {}));
    // BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " "
    //                         << __LINE__ << " new handle:" << (void*)key <<"; mapsize="<< map->size()
    //                         << "; map=" << (void*)map;

    return key;
}

int __compat_handle_reg_fd(int fd, std::string path, std::string findPattern) {
    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();

    auto it = map->find(handle);
    if(it!=map->end()){
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::FILE;
        it->second.ctx.fileInfo.fd = fd;
        strncpy(it->second.ctx.fileInfo.path, path.c_str(),
            sizeof(it->second.ctx.fileInfo.path));
        strncpy(it->second.ctx.fileInfo.findPattern, findPattern.c_str(),
            sizeof(it->second.ctx.fileInfo.findPattern));
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }

    MAP_UNLOCK();
    return handle;
}

void __compat_handle_update_fd(int handle, int fd, std::string path) {

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        it->second.ctx.fileInfo.fd = fd;
        strncpy(it->second.ctx.fileInfo.path, path.c_str(),
            sizeof(it->second.ctx.fileInfo.path));
    }

    MAP_UNLOCK();
}

int __compat_handle_get_fd(int handle) {

    MAP_LOCK();

    int fd = -1;
    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        fd = it->second.ctx.fileInfo.fd;
    }
    MAP_UNLOCK();
    return fd;

}

std::string __compat_handle_get_fd_path(int handle) {

    std::string ret = "";
    MAP_LOCK();


    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        ret = std::string(it->second.ctx.fileInfo.path);
    }
    MAP_UNLOCK();

    return ret;
}

std::string __compat_handle_get_fd_findPattern(int handle) {

    std::string ret = "";
    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        ret = std::string(it->second.ctx.fileInfo.findPattern);
    }

    MAP_UNLOCK();
    return ret;

}

int __compat_handle_reg_module(void* native_handle, std::string full_path) {

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::MODULE;
        it->second.ctx.moduleInfo.nativeHandle = native_handle;
        strncpy(it->second.ctx.moduleInfo.fullPath, full_path.c_str(),
            sizeof(it->second.ctx.moduleInfo.fullPath));
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }
    MAP_UNLOCK();

    return handle;

}

void* __compat_handle_get_module_native_handle(int handle) {

    MAP_LOCK();

    void* ret = nullptr;
    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        ret = it->second.ctx.moduleInfo.nativeHandle;
    }

    MAP_UNLOCK();

    return ret;

}

std::string __compat_handle_get_module_full_path(int handle) {
    MAP_LOCK();

    std::string ret = "";
    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        ret = std::string(it->second.ctx.moduleInfo.fullPath);
    }
    MAP_UNLOCK();

    return ret;

}

int __compat_handle_find_module_by_native_handle(void* native_handle) {
    MAP_LOCK();
    int ret = 0;
    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::MODULE
            && entry.second.ctx.moduleInfo.nativeHandle == native_handle) {
            ret = entry.first;

            break;
        }
    }
    MAP_UNLOCK();
    return ret;
}

int __compat_handle_reg_thread(pthread_t tid) {

    MAP_LOCK();
    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::THREAD;
        it->second.ctx.threadInfo.tid = tid;
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }
    MAP_UNLOCK();
    return handle;

}

pthread_t __compat_handle_get_thread_tid(int handle) {

    MAP_LOCK();
    int tid = 0;
    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        tid = it->second.ctx.threadInfo.tid;
    }
    MAP_UNLOCK();

    if( tid == 0 ){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found tid" ;
    }

    return tid;


}

int __compat_handle_find_thread_by_tid(pthread_t tid) {

    MAP_LOCK();

    int ret = 0;
    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::THREAD
            && entry.second.ctx.threadInfo.tid == tid) {
            ret = entry.first;
        }
    }

    MAP_UNLOCK();

    return ret;
}

bool __compat_handle_set_thread_exitcode(pthread_t tid, unsigned long dwExitCode)
{

    MAP_LOCK();

    bool ret = false;
    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::THREAD
            && entry.second.ctx.threadInfo.tid == tid) {
            entry.second.ctx.threadInfo.exitCode = dwExitCode;
            ret = true;

            break;
        }
    }

    MAP_UNLOCK();

    return ret;
}
unsigned long __compat_handle_get_thread_exitcode(pthread_t tid)
{


    MAP_LOCK();

    unsigned long exitCode = 0;
    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::THREAD
            && entry.second.ctx.threadInfo.tid == tid) {
            exitCode = entry.second.ctx.threadInfo.exitCode;

            break;
        }
    }


    MAP_UNLOCK();

    return exitCode;
}

#if !USE_BOOST_MUTEX
int __compat_handle_mutex_reg(void* mutex, void* attr, std::string name)
{
    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::MUTEX;
        strncpy(it->second.ctx.mutexInfo.name, name.c_str(),
            sizeof(it->second.ctx.mutexInfo.name));
        it->second.ctx.mutexInfo.mutex = mutex;
        it->second.ctx.mutexInfo.attr = attr;
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }
    MAP_UNLOCK();
    return handle;
}
int __compat_handle_mutex_find_by_name(std::string name)
{
    int ret = 0;
	auto tstart = std::chrono::high_resolution_clock::now();

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::MUTEX
            && std::string(entry.second.ctx.mutexInfo.name) == name) {
            ret = entry.first;
            break;
        }
    }

    MAP_UNLOCK();

//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return ret;
}
void* __compat_handle_mutex_get(int handle)
{
    void* mutex = NULL;

    MAP_LOCK();

	auto tstart = std::chrono::high_resolution_clock::now();

    try{
        auto map = __compat_handle_map_get();
        auto it = map->find(handle);
        if (it != map->end()) {
            mutex = it->second.ctx.mutexInfo.mutex;
        }
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << "; handle=" << (void*)handle << " e=" << e.what();
    }

    MAP_UNLOCK();


//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return mutex;
}
void* __compat_handle_mutex_get_attr(int handle)
{
    void* attr = NULL;

    MAP_LOCK();

	auto tstart = std::chrono::high_resolution_clock::now();

    try{
        auto map = __compat_handle_map_get();
        auto it = map->find(handle);
        if (it != map->end()) {
            attr = it->second.ctx.mutexInfo.attr;
        }
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << "; handle=" << (void*)handle << " e=" << e.what();
    }

    MAP_UNLOCK();


//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return attr;
}

std::string __compat_handle_mutex_get_name(int handle)
{
    std::string name = "";

    MAP_LOCK();

	auto tstart = std::chrono::high_resolution_clock::now();

    try{
        auto map = __compat_handle_map_get();
        auto it = map->find(handle);
        if (it != map->end()) {
            name = std::string(it->second.ctx.mutexInfo.name);
        }
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << "; handle=" << (void*)handle << " e=" << e.what();
    }

    MAP_UNLOCK();


//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return name;
}
#else
int __compat_handle_reg_mutex(std::string name) {

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    auto it = map->find(handle);
    if (it != map->end()) {
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::MUTEX;
        strncpy(it->second.ctx.mutexInfo.name, name.c_str(),
            sizeof(it->second.ctx.mutexInfo.name));
        it->second.ctx.mutexInfo.recursiveCount = 0;
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }
    MAP_UNLOCK();
    return handle;
}

std::string __compat_handle_get_mutex_name(int handle) {
    std::string name = "";

    MAP_LOCK();

	auto tstart = std::chrono::high_resolution_clock::now();

    try{
        auto map = __compat_handle_map_get();
        auto it = map->find(handle);
        if (it != map->end()) {
            name = std::string(it->second.ctx.mutexInfo.name);
        }
    }catch(const std::exception& e){
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << "; handle=" << (void*)handle << " e=" << e.what();
    }

    MAP_UNLOCK();


//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return name;
}

int __compat_handle_find_mutex_by_name(std::string name) {
    int ret = 0;
	auto tstart = std::chrono::high_resolution_clock::now();

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::MUTEX
            && std::string(entry.second.ctx.mutexInfo.name) == name) {
            ret = entry.first;
            break;
        }
    }

    MAP_UNLOCK();

//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return ret;
}
#endif
// void __compat_handle_inc_mutex_count(int handle) {
//     auto map = __compat_handle_map_get();
//     if (map->find(handle) != map->end()) {
//         map->at(handle).ctx.mutexInfo.recursiveCount
//             = map->at(handle).ctx.mutexInfo.recursiveCount + 1;
//     }
// }

// void __compat_handle_dec_mutex_count(int handle) {
//     auto map = __compat_handle_map_get();
//     if (map->find(handle) != map->end()) {
//         map->at(handle).ctx.mutexInfo.recursiveCount
//             = map->at(handle).ctx.mutexInfo.recursiveCount - 1;
//     }
// }

// bool __compat_handle_get_mutex_unlockable(int handle) {
//     bool ret = false;
//     auto map = __compat_handle_map_get();
//     if (map->find(handle) != map->end()) {
//         if (map->at(handle).ctx.mutexInfo.recursiveCount > 0) {
//             ret = true;
//         }
//     }
//     return ret;
// }
#if USE_BOOST_EVENT
int __compat_handle_reg_event(
    std::string name, bool bManualReset, bool bSignaled) {
    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    map->at(handle).selfKey = handle;
    map->at(handle).type = __compat_handle_type_t::EVENT;
    map->at(handle).ctx.eventInfo.bManualReset = bManualReset;
    map->at(handle).ctx.eventInfo.bSignaled = bSignaled;
    strncpy(map->at(handle).ctx.eventInfo.name, name.c_str(),
        sizeof(map->at(handle).ctx.eventInfo.name));
    return handle;
}

std::string __compat_handle_get_event_name(int handle) {
    std::string name = "";
    auto map = __compat_handle_map_get();
    if (map->find(handle) != map->end()) {
        name = std::string(map->at(handle).ctx.eventInfo.name);
    }
    return name;
}

bool __compat_handle_get_event_b_manual_reset(int handle) {
    bool bManualReset = false;
    auto map = __compat_handle_map_get();
    if (map->find(handle) != map->end()) {
        bManualReset = map->at(handle).ctx.eventInfo.bManualReset;
    }
    return bManualReset;
}

void __compat_handle_set_event_status(int handle, bool bSignaled) {
    auto map = __compat_handle_map_get();
    if (map->find(handle) != map->end()) {
        map->at(handle).ctx.eventInfo.bSignaled = bSignaled;
    }
}

bool __compat_handle_get_event_status(int handle) {
    bool bSignaled = false;
    auto map = __compat_handle_map_get();
    if (map->find(handle) != map->end()) {
        bSignaled = map->at(handle).ctx.eventInfo.bSignaled;
    }
    return bSignaled;
}

int __compat_handle_find_event_by_name(std::string name) {
    int ret = 0;
    auto map = __compat_handle_map_get();
    for (auto& entry : *map) {
        if (entry.second.type == __compat_handle_type_t::EVENT
            && std::string(entry.second.ctx.eventInfo.name) == name) {
            ret = entry.first;
        }
    }
    return ret;
}


#else
int __compat_handle_reg_event( void* hEvent, bool bManualReset, bool bSignaled)
{

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto handle = __compat_handle_new_key_get();
    auto it = map->find(handle);
    if(it != map->end()){
        it->second.selfKey = handle;
        it->second.type = __compat_handle_type_t::EVENT;
        it->second.ctx.eventInfo.bManualReset = bManualReset;
        it->second.ctx.eventInfo.bSignaled = bSignaled;
        it->second.ctx.eventInfo.hEvent = hEvent;

    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                            << __LINE__ << " not found handle:" << handle ;
    }
    MAP_UNLOCK();

    return handle;    

}

void* __compat_handle_get_event(int handle) {
	void* hEvent = NULL;

    MAP_LOCK();

	auto tstart = std::chrono::high_resolution_clock::now();

    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if(it != map->end()){
    	hEvent = it->second.ctx.eventInfo.hEvent;
    }

    MAP_UNLOCK();
//    auto tend = std::chrono::high_resolution_clock::now();
//    if( count%10000 == 0){
//		BOOST_LOG_TRIVIAL(warning)
//			<< __FILE__ << " " << __FUNCTION__
//			<< " " << __LINE__ << "; time="
//			<< std::chrono::duration_cast<
//				   std::chrono::microseconds>(  tend - tstart).count();
//    }
    return hEvent;
}

#endif

__compat_handle_type_t __compat_handle_type_get(int handle) {
    auto type = __compat_handle_type_t::INVALID;


    MAP_LOCK();

    auto map = __compat_handle_map_get();
    auto it = map->find(handle);
    if(it != map->end()){
        type = it->second.type;
    }

    MAP_UNLOCK();

    return type;
}

void __compat_handle_erase(int handle) {

    MAP_LOCK();

    auto map = __compat_handle_map_get();
    if (map->find(handle) != map->end()) {
        map->erase(handle);
        // BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " "
        //             << __LINE__ << " erase handle:" << (void*)handle <<"; mapsize="<< map->size() 
        //             << "; map=" << (void*)map;
    }

    MAP_UNLOCK();

}

size_t __compat_handle_object_count() {
    auto map = __compat_handle_map_get();
    return map->size();
}

int __compat_handle_print() {

    auto map = __compat_handle_map_get();
    for (auto iter = map->begin(); iter != map->end(); iter++) {
        switch (iter->second.type) {
        case __compat_handle_type_t::MODULE:
            std::cout << (void*)iter->first << "; selfkey=" << (void*)iter->second.selfKey
                      << ": module: " << iter->second.ctx.moduleInfo.fullPath
                      << std::endl;
            break;
        case __compat_handle_type_t::MUTEX:
            std::cout << (void*)iter->first << "; selfkey=" << (void*)iter->second.selfKey
                      << ": mutex: " << iter->second.ctx.mutexInfo.name
                      << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::EVENT:
            std::cout << (void*)iter->first << "; selfkey=" << (void*)iter->second.selfKey
#if !USE_BOOST_EVENT
                      << ": event: " << iter->second.ctx.eventInfo.hEvent
#else

#endif
                      << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::THREAD:
            std::cout << (void*)iter->first << "; selfkey=" << (void*)iter->second.selfKey
                      << ": thread: " << (void*)iter->second.ctx.threadInfo.tid << "; exitcode=" << iter->second.ctx.threadInfo.exitCode << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::FILE:
            std::cout << (void*)iter->first << "; selfkey=" << (void*)iter->second.selfKey
                      << ": file: " << iter->second.ctx.fileInfo.fd << "; path: " << iter->second.ctx.fileInfo.path
                      << std::endl;
            /* code */
            break;
        default:
            break;
        }
    }

    return 0;
}
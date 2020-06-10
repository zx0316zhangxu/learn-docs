#if !defined(__COMMON_HANDLE_H__)
#define __COMMON_HANDLE_H__

#include <stdint.h>
#include <utility>
#include <string>
#include <pthread.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#define USE_BOOST_EVENT	0
#define USE_BOOST_MUTEX	0
#define USE_BOOST_MAP   0

enum class __compat_handle_type_t {
    DUMMY_START = 0,
    // user
    ACCELERATOR_TABLE,
    CARET,
    CURSOR,
    DDE_CONVERSATION,
    HOOK,
    ICON,
    MENU,
    WINDOW,
    WINDOW_POSITION,
    // gdi
    BITMAP,
    BRUSH,
    DC,
    ENHANCED_METAFILE,
    ENHANCED_METAFILE_DC,
    FONT,
    MEMORY_DC,
    METAFILE,
    METAFILE_DC,
    PALETTE,
    PEN_AND_EXTENDED_PEN,
    REGION,
    // kernel
    ACCESS_TOKEN,
    CHANGE_NOTIFICATION,
    COMMUNICATIONS_DEVICE,
    CONSOLE_INPUT,
    CONSOLE_SCREEN_BUFFER,
    DESKTOP,
    DIRECTORY,
    EVENT,
    EVENT_LOG,
    FILE,
    FILE_MAPPING,
    HEAP,
    JOB,
    MAILSLOT,
    MODULE,
    MUTEX,
    PIPE,
    PROCESS,
    REGISTRY_KEY,
    SEMAPHORE,
    SOCKET,
    THREAD,
    TIMER,
    TIMER_QUEUE,
    TIMER_QUEUE_TIMER,
    UPDATE_RESOURCE,
    WINDOW_STATION,
    //
    DUMMY_END,
    //
    INVALID,
};


class __compat_handle_header_t {
public:
    int selfKey;
    __compat_handle_type_t type;
    union {
        struct{
            pthread_t tid;
            unsigned long exitCode;
        } threadInfo;
        struct {
            int fd;
            char path[UINT8_MAX];
            char findPattern[UINT8_MAX];
        } fileInfo;
        struct {
            void* nativeHandle;
            char fullPath[UINT8_MAX];
        } moduleInfo;
        struct {
#if !USE_BOOST_MUTEX
            void* mutex;
            void* attr;
            char name[UINT8_MAX];
#else
            char name[UINT8_MAX];
            int recursiveCount;
#endif
        } mutexInfo;
        struct {
#if !USE_BOOST_EVENT
            void* hEvent;
#else
        	char name[UINT8_MAX];
#endif
            bool bSignaled;
            bool bManualReset;
        } eventInfo;
    } ctx;
};

#if !USE_BOOST_MAP
#include <unordered_map>

typedef std::unordered_map<int, __compat_handle_header_t> __compat_handle_map_t;

// typedef std::pair<const int, __compat_handle_header_t> __compat_handle_key_t;
// typedef boost::interprocess::allocator<__compat_handle_key_t,
//     boost::interprocess::managed_shared_memory::segment_manager>
//     __compat_handle_map_shared_mem_allocator_t;
// typedef boost::unordered_map<int, __compat_handle_header_t, std::less<int>,
//     __compat_handle_map_shared_mem_allocator_t>
//     __compat_handle_map_t;

#else

typedef std::pair<const int, __compat_handle_header_t> __compat_handle_key_t;
typedef boost::interprocess::allocator<__compat_handle_key_t,
    boost::interprocess::managed_shared_memory::segment_manager>
    __compat_handle_map_shared_mem_allocator_t;
typedef boost::interprocess::map<int, __compat_handle_header_t, std::less<int>,
    __compat_handle_map_shared_mem_allocator_t>
    __compat_handle_map_t;
#endif

const auto __compat_handle_region_name = u8"__compat_handle_region";
const auto __compat_handle_map_name = u8"__compat_handle_map";
const auto __compat_handle_region_size = 1048576UL * 16UL;

__compat_handle_map_t* __compat_handle_map_get();
int __compat_handle_new_key();

// file
int __compat_handle_reg_fd(
    int fd, std::string path, std::string findPattern = std::string(""));
void __compat_handle_update_fd(int handle, int fd, std::string path);
int __compat_handle_get_fd(int handle);
std::string __compat_handle_get_fd_path(int handle);
std::string __compat_handle_get_fd_findPattern(int handle);

// module (dyn lib)
int __compat_handle_reg_module(void* native_handle, std::string full_path);
void* __compat_handle_get_module_native_handle(int handle);
std::string __compat_handle_get_module_full_path(int handle);
int __compat_handle_find_module_by_native_handle(void* native_handle);

// thread
int __compat_handle_reg_thread(pthread_t tid);
pthread_t __compat_handle_get_thread_tid(int handle);
int __compat_handle_find_thread_by_tid(pthread_t tid);
bool __compat_handle_set_thread_exitcode(pthread_t tid, unsigned long dwExitCode);
unsigned long __compat_handle_get_thread_exitcode(pthread_t tid);

// mutex
#if !USE_BOOST_MUTEX
int __compat_handle_mutex_reg(void* mutex, void* attr = NULL, std::string name = std::string(""));
int __compat_handle_mutex_find_by_name(std::string name);
void* __compat_handle_mutex_get(int handle);
void* __compat_handle_mutex_get_attr(int handle);
std::string __compat_handle_mutex_get_name(int handle);
#else
int __compat_handle_reg_mutex(std::string name);
std::string __compat_handle_get_mutex_name(int handle);
int __compat_handle_find_mutex_by_name(std::string name);
// void __compat_handle_inc_mutex_count(int handle);
// void __compat_handle_dec_mutex_count(int handle);
// bool __compat_handle_get_mutex_unlockable(int handle);
#endif

// event
#if USE_BOOST_EVENT
int __compat_handle_reg_event(
    std::string name, bool bManualReset, bool bSignaled);
std::string __compat_handle_get_event_name(int handle);
bool __compat_handle_get_event_b_manual_reset(int handle);
void __compat_handle_set_event_status(int handle, bool bSignaled);
bool __compat_handle_get_event_status(int handle);
int __compat_handle_find_event_by_name(std::string name);
#else
int __compat_handle_reg_event(
    void* hEvnet, bool bManualReset, bool bSignaled);
void* __compat_handle_get_event(int handle);
#endif

__compat_handle_type_t __compat_handle_type_get(int handle);

void __compat_handle_erase(int handle);

size_t __compat_handle_object_count();

int __compat_handle_print();

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
} /* ... extern "C" */
#endif /* __cplusplus */

#endif // __COMMON_HANDLE_H__

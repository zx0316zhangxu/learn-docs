#include <iostream>
#include "common_handle.h"

int main() {


    auto map = __compat_handle_map_get();
    for (auto iter = map->begin(); iter != map->end(); iter++) {
        switch (iter->second.type) {
        case __compat_handle_type_t::MODULE:
            std::cout << (void*)iter->second.selfKey
                      << ": module: " << iter->second.ctx.moduleInfo.fullPath
                      << std::endl;
            break;
        case __compat_handle_type_t::MUTEX:
            std::cout << (void*)iter->second.selfKey
                      << ": mutex: " << iter->second.ctx.mutexInfo.name
                      << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::EVENT:
            std::cout << (void*)iter->second.selfKey
#if !USE_BOOST_EVENT
                      << ": event: " << iter->second.ctx.eventInfo.hEvent
#else

#endif
                      << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::THREAD:
            std::cout << (void*)iter->second.selfKey
                      << ": thread: " << (void*)iter->second.ctx.threadInfo.tid << "; exitcode=" << iter->second.ctx.threadInfo.exitCode << std::endl;
            /* code */
            break;
        case __compat_handle_type_t::FILE:
            std::cout << (void*)iter->second.selfKey
                      << ": file: " << (void*)iter->second.ctx.fileInfo.fd << "; path: " << iter->second.ctx.fileInfo.path
                      << std::endl;
            /* code */
            break;
        default:
            break;
        }
    }
    boost::interprocess::shared_memory_object::remove(
        __compat_handle_region_name);


    return 0;
}

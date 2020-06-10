#include <boost/thread.hpp>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <boost/log/trivial.hpp>
#include <locale>
#include <codecvt>
#include "common_handle.h"
#include "process.h"
#include "processthreadsapi.h"

void ExitThread(DWORD dwExitCode) {
//	try{
		auto tid = pthread_self();

#if 0
		if(0!=pthread_cancel(tid))
		{
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
									 << __LINE__ << " pthread_exit::failed" ;
		}

		__compat_handle_set_thread_exitcode(tid, dwExitCode);

		BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid << "; ExitCode=" << __compat_handle_get_thread_exitcode(tid);;
#else
		__compat_handle_set_thread_exitcode(tid, dwExitCode);

		BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid << "; ExitCode=" << __compat_handle_get_thread_exitcode(tid);;
		pthread_exit(0);
#endif
		// pthread_exit((void*)dwExitCode);
//	}catch (const std::exception& e) {
//		BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
//								 << __LINE__ << " " << e.what();
//	}
}

// TODO: ???
DWORD SuspendThread(HANDLE hThread) {
    auto ret = -1;
    auto tid = __compat_handle_get_thread_tid((int)hThread);
    if (tid != 0) {
        if(0!=pthread_cancel(tid))
        {
            BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                     << __LINE__ << " pthread_cancel::failed" ;
        }
    }else{
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " not found tid" ;
    }
    BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid;
    return ret;
}

DWORD GetCurrentThreadId() {
    auto ret = 0;
    auto currentHandle = __compat_handle_find_thread_by_tid(pthread_self());
    if (currentHandle == 0) {
        ret = __compat_handle_reg_thread(pthread_self());
    }
    else {
        ret = currentHandle;
    }
    return ret;
}

// lpThreadAttributes ignored, always NULL
// dwStackSize ignored, always 0
// dwCreationFlags ignored, always 0
HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
    size_t dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
    if (lpStartAddress == NULL) {
        return NULL;
    }
    auto handle = _beginthread(
        (void (*)(void*))lpStartAddress, dwStackSize, lpParameter);
    if (lpThreadId != NULL) {
        *lpThreadId = (DWORD)handle;

        auto tid = __compat_handle_get_thread_tid((int)handle);
        __compat_handle_set_thread_exitcode(tid, STILL_ACTIVE);
        BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid;
    }
    return (HANDLE)handle;
}

void SetScheduler(int policy)
{

    int ret =0;
    pid_t pid =getpid();

    int curschdu = sched_getscheduler(pid);

    if(curschdu!=policy){
        struct sched_param s_parm;
        s_parm.sched_priority = sched_get_priority_max(policy) / 2;
        ret = sched_setscheduler(pid, policy, &s_parm);
    }

}

int GetThreadPriority(HANDLE hThread)
{
    struct sched_param sp;
    bzero((void*)&sp, sizeof(sp));
    int policy = SCHED_OTHER;
    auto tid = __compat_handle_get_thread_tid((int)hThread);

	pthread_getschedparam ((pthread_t)tid, &policy, &sp);

	return sp.sched_priority;
}

// TODO: not implemented
BOOL SetThreadPriority(HANDLE hThread, int nPriority) { 
    
    // Start out with a standard, low-priority setup for the sched params.
    struct sched_param sp;
    bzero((void*)&sp, sizeof(sp));
    int policy = SCHED_OTHER;
    auto tid = __compat_handle_get_thread_tid((int)hThread);
    if( tid == 0 ){
        return FALSE;
    }

    policy = SCHED_RR;

    SetScheduler(policy);

    int priority = (sched_get_priority_max(policy) + sched_get_priority_min(policy)) / 2;

    // If desired, set up high-priority sched params structure.
    switch (nPriority) {
        case THREAD_PRIORITY_LOWEST:
            priority = sched_get_priority_min(policy);
            break;
        case THREAD_PRIORITY_BELOW_NORMAL:
            priority = sched_get_priority_min(policy) + 1;
            break;
        case THREAD_PRIORITY_NORMAL:
            priority = (sched_get_priority_max(policy) + sched_get_priority_min(policy)) / 2;
            break;
        case THREAD_PRIORITY_HIGHEST:
            priority = sched_get_priority_max(policy);
            break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
            priority = sched_get_priority_max(policy) - 1;
            break;
        default:
            break;
    }
    sp.sched_priority = priority;
    // Actually set the sched params for the current thread.
    auto result = pthread_setschedparam(tid, policy, &sp);
    if (0 != result) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " tid=" << (void*)tid << "pthread:" << (void*)pthread_self() << "; result=" << result << "; pri=" << sp.sched_priority;
    }else{
        BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " tid=" << (void*)tid << "pthread:" << (void*)pthread_self() << "; result=" << result << "; pri=" << GetThreadPriority(hThread);
    }
    
}

// TODO: ???
BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode) {
    auto tid = __compat_handle_get_thread_tid((int)hThread);
    BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << (void*)tid;
    if (tid != 0) {
        if(0!=pthread_cancel(tid)){
			BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
									 << __LINE__ << " pthread_cancel::failed" ;
        }else{
            __compat_handle_set_thread_exitcode(tid, dwExitCode);
        }
        return TRUE;
    }
    else {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                 << __LINE__ << " not found tid" ;
        return FALSE;
    }
}

// TODO: not implemented
BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode) {
    auto tid = __compat_handle_get_thread_tid((int)hThread);

    if (lpExitCode && (tid != 0)) {
        *lpExitCode = __compat_handle_get_thread_exitcode(tid);
    }
    return TRUE;
}

HANDLE GetCurrentThread() {
    return (HANDLE)__compat_handle_find_thread_by_tid(pthread_self());
}

BOOL CreateProcessA(
  LPCSTR                lpApplicationName,
  LPSTR                 lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPCSTR                lpCurrentDirectory,
  LPSTARTUPINFOA        lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
) {

    BOOST_LOG_TRIVIAL(info) << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "; lpApplicationName=" << lpApplicationName << "; lpCommandLine=" << lpCommandLine ;

    if (lpProcessAttributes || lpThreadAttributes || bInheritHandles || dwCreationFlags || lpEnvironment || lpCurrentDirectory) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                        << __LINE__ << " ##TODO##" ;
        return FALSE;
    }
    if (!lpApplicationName) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                        << __LINE__ << " Not supported" ;
        return FALSE;
    }
    auto pid = fork();
    if (pid == 0) {
        execlp(lpApplicationName, lpCommandLine);
        exit(errno);
        return FALSE;
    }
    else {
        if (lpProcessInformation) {
            lpProcessInformation->hProcess = (HANDLE)pid;
        }
        return TRUE;
    }
}

BOOL CreateProcessW(
  LPCWSTR               lpApplicationName,
  LPWSTR                lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPCWSTR               lpCurrentDirectory,
  LPSTARTUPINFOW        lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
) {

    if (lpProcessAttributes || lpThreadAttributes || bInheritHandles || dwCreationFlags || lpEnvironment || lpCurrentDirectory) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                        << __LINE__ << " ##TODO##" ;
        return FALSE;
    }
    if (!lpApplicationName) {
        BOOST_LOG_TRIVIAL(error) << __FILE__ << " " << __FUNCTION__ << " "
                                        << __LINE__ << " Not supported" ;
        return FALSE;
    }

    STARTUPINFOA startInfo;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    auto sApplicationName = cvt.to_bytes(lpApplicationName);
    std::string sCommandLine = "";
    if(lpCommandLine != NULL){
        sCommandLine = cvt.to_bytes(lpCommandLine);
    }
    return CreateProcessA(sApplicationName.c_str(), (LPSTR)sCommandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startInfo, lpProcessInformation);
}

#ifdef LINUX_TODO

BOOL GetExitCodeProcess(HANDLE hProcess,LPDWORD lpExitCode) {
    int status;
    *lpExitCode = 0;
    if (waitpid((pid_t)hProcess, (int*)lpExitCode, WUNTRACED | WCONTINUED) == -1) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode) {
    if (kill((pid_t)hProcess, 9) == -1) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

#endif

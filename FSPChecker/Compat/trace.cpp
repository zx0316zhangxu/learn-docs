#include <windows.h>
#include <tchar.h>

#if 0
extern "C" void __attribute__((no_instrument_function))
__cyg_profile_func_enter(void* this_func, void* call_site) {
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "enter func: " << this_func << ", caller: " << call_site;
}

extern "C" void __attribute__((no_instrument_function))
__cyg_profile_func_exit(void* this_func, void* call_site) {
    BOOST_LOG_TRIVIAL(trace)
        << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " "
        << "exit func: " << this_func << ", caller: " << call_site;
}
#endif


char* GetLogSystemTime()
{
    SYSTEMTIME	SysTime;
	DWORD		dwRc;
	static char		tchBuff[1024];
//	TCHAR		tchApp[16];
	BOOL		bRc;

	//Date/Time
	::ZeroMemory(&SysTime, sizeof(SYSTEMTIME));
	::GetLocalTime(&SysTime);

	////Server/Client
	//::ZeroMemory(&tchApp[0], sizeof(tchApp));
	//if(Utility::m_dwApp == Utility::APP_SERVER){
	//	_tcscpy_s(&tchApp[0], _countof(tchApp), _T(APP_SERVER_NM));
	//}
	//else{
	//	_tcscpy_s(&tchApp[0], _countof(tchApp), _T(APP_CLIENT_NM));
	//}

	//書式設定
	::ZeroMemory(&tchBuff[0] ,sizeof(tchBuff));

	sprintf((char*)&tchBuff[0], 
				"%04d/%02d/%02d %02d:%02d:%02d %03d", 
				SysTime.wYear,
				SysTime.wMonth,
				SysTime.wDay,
				SysTime.wHour,
				SysTime.wMinute,
				SysTime.wSecond,
			    SysTime.wMilliseconds
    );

    return tchBuff;
}

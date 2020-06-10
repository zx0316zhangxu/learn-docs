#include "windows.h"
#include <sstream>
#include <iostream>

#include <fstream>

#ifdef __linux__

const char* FAT_ROOT_PATH = "/SDMemory2/";
const char* NEXT_ROOT_PATH = "/SDMemory3/";

#else
const char* FAT_ROOT_PATH = "\\SDMemory2\\";
const char* NEXT_ROOT_PATH = "\\SDMemory3\\";
#endif
const char* CPP_FILE_NAME = "CPP_FILE.bin";
const char* CPP_DB_NAME = "CPP_DB.db";
const char* CPP_LOG_NAME = "CPP_LOG.txt";
char APP_ROOT_PATH[128] = "";

using namespace std;
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

	//èëéÆê›íË
	::ZeroMemory(&tchBuff[0], sizeof(tchBuff));

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
void WriteLogBuffer(const char* buff)
{
     ofstream fs = std::ofstream(CPP_LOG_NAME, std::ios::out | std::ios::app);
     fs << GetLogSystemTime() << " " << buff << std::endl;
     fs.close();

	 cout << GetLogSystemTime() << " " << buff << std::endl;
}
#pragma once

#include <windows.h>
#include "sqlite3.h"


#if !defined(__SQLITE3INSERT__)
#define __SQLITE3INSERT__

#include "windows.h"

/*!
	@brief	ファイル入出力クラス
*/
class Sqlite3Insert
{
public:
	Sqlite3Insert(void) {};						//コンストラクタ
	~Sqlite3Insert(void) {};				//デストラクタ

	//FileIO
	BOOL	Excute(const char* root_path);

private:
	HMODULE					SQLite3;				// SQLITE3ハンドル

#ifdef __linux__
	typedef int (* pOpen)(char *, sqlite3 **);
	pOpen			Open;
	typedef int (* pExec)(sqlite3 *, const char *, sqlite3_callback, void *, char **);
	pExec			Exec;
	typedef int (* pGetTable)(sqlite3 *, const char *, char ***, int *, int *, char **);
	pGetTable		GetTable;
	typedef int (* pFreeTable)(char **);
	pFreeTable		FreeTable;
	typedef int (* pClose)(sqlite3 *);
	pClose			Close;
	typedef int (*pStep)(sqlite3_stmt *);
	pStep			Step;
	typedef int (*pReset)(sqlite3_stmt *);
	pReset			Reset;
	typedef int (*pFinalize)(sqlite3_stmt *);
	pFinalize		Finalize;
	typedef int (* pbind_blob)(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
	pbind_blob		Setblob;
	typedef int (* pbind_double)(sqlite3_stmt*, int, double);
	pbind_double	Setdouble;
	typedef int (* pbind_int)(sqlite3_stmt*, int, int);
	pbind_int		Setint;
	typedef int (* pbind_int64)(sqlite3_stmt*, int, sqlite3_int64);
	pbind_int64		Setint64;
	typedef int (* pbind_null)(sqlite3_stmt*, int);
	pbind_null		Setnull;
	typedef int (* pbind_text)(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
	pbind_text		Settext;
	typedef int (* pPrepare)( sqlite3 *,const char *, int, sqlite3_stmt **, const char **);
	pPrepare		Prepare;
	typedef int (* pColumn_int)(sqlite3_stmt*, int);
	pColumn_int		Int;
	typedef int (* pError)(sqlite3 *);
	pError		Error;
	typedef int (* pClearBindings)(void *);
	pClearBindings		ClearBindings;
	typedef int (* pReleaseMemory)(sqlite3 *);
	pReleaseMemory		ReleaseMemory;
	typedef const char* (* pColumn_text)(sqlite3_stmt*, int);
#else

	// APIポインタ
	typedef int(__cdecl* pOpen)(char*, sqlite3**);
	pOpen			Open;
	typedef int(__cdecl* pExec)(sqlite3*, const char*, sqlite3_callback, void*, char**);
	pExec			Exec;
	typedef int(__cdecl* pGetTable)(sqlite3*, const char*, char***, int*, int*, char**);
	pGetTable		GetTable;
	typedef int(__cdecl* pFreeTable)(char**);
	pFreeTable		FreeTable;
	typedef int(__cdecl* pClose)(sqlite3*);
	pClose			Close;
	typedef int(_cdecl* pStep)(sqlite3_stmt*);
	pStep			Step;
	typedef int(_cdecl* pReset)(sqlite3_stmt*);
	pReset			Reset;
	typedef int(_cdecl* pFinalize)(sqlite3_stmt*);
	pFinalize		Finalize;
	typedef int(__cdecl* pbind_blob)(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
	pbind_blob		Setblob;
	typedef int(__cdecl* pbind_double)(sqlite3_stmt*, int, double);
	pbind_double	Setdouble;
	typedef int(__cdecl* pbind_int)(sqlite3_stmt*, int, int);
	pbind_int		Setint;
	typedef int(__cdecl* pbind_int64)(sqlite3_stmt*, int, sqlite3_int64);
	pbind_int64		Setint64;
	typedef int(__cdecl* pbind_null)(sqlite3_stmt*, int);
	pbind_null		Setnull;
	typedef int(__cdecl* pbind_text)(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
	pbind_text		Settext;
	typedef int(_cdecl* pPrepare)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
	pPrepare		Prepare;
	typedef int(_cdecl* pColumn_int)(sqlite3_stmt*, int);
	pColumn_int		Int;
	typedef int(__cdecl* pError)(sqlite3*);
	pError		Error;
	typedef int(__cdecl* pClearBindings)(void*);
	pClearBindings		ClearBindings;
	typedef int(__cdecl* pReleaseMemory)(sqlite3*);
	pReleaseMemory		ReleaseMemory;
	typedef const char* (_cdecl* pColumn_text)(sqlite3_stmt*, int);
#endif

};
#endif		//__FILE_WRITEREAD__


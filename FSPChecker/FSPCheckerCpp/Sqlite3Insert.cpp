#include "windows.h"
#include "stdio.h"
#include "Sqlite3Insert.h"
#include "Log.h"
#include "tchar.h"

FARPROC GetProcAddressWrap(HMODULE hModule, LPCWSTR lpProcName)
{
	FARPROC result = NULL;
	try
	{
#ifdef WINXP
		// LPCWSTR -> LPCSTR‚ÖƒLƒƒƒXƒg
		char apiName[128];
		memset(apiName, 0, sizeof(apiName));
		size_t wLen = 0;
		//setlocale(LC_ALL, "English");
		errno_t err = wcstombs_s(&wLen, apiName, sizeof(apiName), lpProcName, _TRUNCATE);
		result = GetProcAddress(hModule, apiName);
#else
		result = GetProcAddress(hModule, lpProcName);
#endif
	}
	catch (...)
	{
		char logMsg[512] = { 0 };
		sprintf(&logMsg[0], "Catch exception! Code[0x%x] -  File=\"%s\", Function=\"%s\"", GetLastError(), __FILE__, __FUNCTION__);
		printf(logMsg);
	}
	return result;
}

BOOL Sqlite3Insert::Excute(const char* root_path)
{
	Open = NULL;
	SQLite3 = NULL;

#ifdef __linux__
		SQLite3 = LoadLibrary(TEXT("libsqlite3.so"));
#else

#ifdef WINXP
	SQLite3 = LoadLibrary(TEXT("sqlite3.dll"));
#else
	SQLite3 = LoadLibrary(TEXT("sqlite3.7.15.2-arm4-wince.dll"));
#endif
#endif

	if ((Open == NULL) && (SQLite3 != NULL))
	{
#ifdef __linux__
		Open = (pOpen)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_open"));
		Exec = (pExec)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_exec"));
		GetTable = (pGetTable)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_get_table"));
		FreeTable = (pFreeTable)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_free_table"));
		Close = (pClose)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_close"));
		Step = (pStep)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_step"));
		Reset = (pReset)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_reset"));
		Finalize = (pFinalize)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_finalize"));
		Setblob = (pbind_blob)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_bind_blob"));
		//Setdouble = (pbind_double)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_double"));
		Setint = (pbind_int)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_bind_int"));
		Setint64 = (pbind_int64)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_bind_int64"));
		Setnull = (pbind_null)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_bind_null"));
		Settext = (pbind_text)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_bind_text"));
		Prepare = (pPrepare)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_prepare"));
		Int = (pColumn_int)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_column_int"));
		Error = (pError)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_errmsg"));
		ClearBindings = (pClearBindings)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_clear_bindings"));
		ReleaseMemory = (pReleaseMemory)GetProcAddressWrap(SQLite3, TEXT("sqlite3n_db_release_memory"));
#else
		Open = (pOpen)GetProcAddressWrap(SQLite3, TEXT("sqlite3_open"));
		Exec = (pExec)GetProcAddressWrap(SQLite3, TEXT("sqlite3_exec"));
		GetTable = (pGetTable)GetProcAddressWrap(SQLite3, TEXT("sqlite3_get_table"));
		FreeTable = (pFreeTable)GetProcAddressWrap(SQLite3, TEXT("sqlite3_free_table"));
		Close = (pClose)GetProcAddressWrap(SQLite3, TEXT("sqlite3_close"));
		Step = (pStep)GetProcAddressWrap(SQLite3, TEXT("sqlite3_step"));
		Reset = (pReset)GetProcAddressWrap(SQLite3, TEXT("sqlite3_reset"));
		Finalize = (pFinalize)GetProcAddressWrap(SQLite3, TEXT("sqlite3_finalize"));
		Setblob = (pbind_blob)GetProcAddressWrap(SQLite3, TEXT("sqlite3_bind_blob"));
		Setdouble = (pbind_double)GetProcAddressWrap(SQLite3, TEXT("sqlite3_double"));
		Setint = (pbind_int)GetProcAddressWrap(SQLite3, TEXT("sqlite3_bind_int"));
		Setint64 = (pbind_int64)GetProcAddressWrap(SQLite3, TEXT("sqlite3_bind_int64"));
		Setnull = (pbind_null)GetProcAddressWrap(SQLite3, TEXT("sqlite3_bind_null"));
		Settext = (pbind_text)GetProcAddressWrap(SQLite3, TEXT("sqlite3_bind_text"));
		Prepare = (pPrepare)GetProcAddressWrap(SQLite3, TEXT("sqlite3_prepare"));
		Int = (pColumn_int)GetProcAddressWrap(SQLite3, TEXT("sqlite3_column_int"));
		Error = (pError)GetProcAddressWrap(SQLite3, TEXT("sqlite3_errmsg"));
		ClearBindings = (pClearBindings)GetProcAddressWrap(SQLite3, TEXT("sqlite3_clear_bindings"));
		ReleaseMemory = (pReleaseMemory)GetProcAddressWrap(SQLite3, TEXT("sqlite3_db_release_memory"));
#endif
	}
	if (Open == NULL) {
		printf("Load Sqlite3 Library Error");
		return FALSE;
	}
	sqlite3* db = NULL;
	char filename[128];
	memset(filename, 0x00, sizeof(filename));
	strcat(filename, root_path);
	strcat(filename, CPP_DB_NAME);
	char* errmsg = NULL;
	int ret; 
	ret = Open(filename, &db);

	if (ret)
	{
		printf("can't open db, errno: %s\n", Error(db));

		Close(db);
		return FALSE;
	}
	char cmd[1024];
	char str[128];

	memset(cmd, 0x00, sizeof(cmd));
	ret = Exec(db, "DROP TABLE IF EXISTS tbTest", NULL, NULL, &errmsg);
	if (ret != SQLITE_OK)
	{
		printf("drop table if exists tbTest failed, errmsg : %s \n", errmsg);
	}

	ret = Exec(db, "CREATE TABLE tbTest(id INTEGER PRIMARY KEY,	name TEXT, price INT)", NULL, NULL, &errmsg);
	if (ret != SQLITE_OK)
	{
		printf("create table  failed, errmsg : %s \n", errmsg);
	}

	sprintf(str, "SQLITE3_INSERT_%s_START", root_path);
	WriteLogBuffer(str);
	for (int i = 0; i < 1000; i++)
	{
		Exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		for (int j = 0; j < 100; j++) {
			memset(cmd, 0x00, sizeof(cmd));

			sprintf(cmd, "INSERT INTO tbTest(name, price) VALUES('Audi',%d);", i*100+j);

			ret = Exec(db, cmd, NULL, NULL, &errmsg);
			if (ret != SQLITE_OK)
			{
				printf("insert  failed, errmsg : %s \n", errmsg);
			}
		}
		Exec(db, "END TRANSACTION;", NULL, NULL, NULL);
	}
	sprintf(str, "SQLITE3_INSERT_%s_END", root_path);
	WriteLogBuffer(str);
	return TRUE;
}
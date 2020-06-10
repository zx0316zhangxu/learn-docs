#pragma once
#if !defined(__LOG_H__)
#define __LOG_H__


extern	const char* FAT_ROOT_PATH;
extern	const char* NEXT_ROOT_PATH;
extern	const char* CPP_FILE_NAME;
extern	const char* CPP_DB_NAME;
extern	const char* CPP_LOG_NAME;
extern	char APP_ROOT_PATH[128];

extern void WriteLogBuffer(const char* buff);

#endif
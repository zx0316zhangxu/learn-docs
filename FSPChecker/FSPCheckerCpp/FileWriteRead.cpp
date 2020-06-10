#include "windows.h"
#include "FileWriteRead.h"
#include <stdio.h>
#include <string.h>
#include "tchar.h"
#include <string>

#ifdef __linux__
#include <unistd.h>
#include <cstdint>
#include <chrono>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <io.h>

#endif

#include "Log.h"


#define WRITE_SIZE (1024 * 1024 * 10)
#define FILE_SIZE (1024 * 1024 * 100)

#ifdef __linux__
long buffer[4096/4] __attribute__((aligned(4096))) ;
#else
long buffer[4096/4];
#endif


BOOL FileWriteRead::Excute(const char* root_path)
{


#ifdef __linux__

	char str[128];
	char filename[128];
	memset(filename, 0x00, sizeof(filename));
	strcat(filename, root_path);
	strcat(filename, CPP_FILE_NAME);
	

	int fd = open(filename, O_RDWR , 0100644); 
	if (-1 == fd)
	{
		printf("open file errno %d\n");;
		return FALSE;
	}
	#if 0
	for (size_t i = 0; i < 1024 * 10; i++)
	{
		buffer[0] = i;
		if (write(fd, buffer, 1024) != 1024)
		{
			printf("write file errnor %d\n", errno);
			return errno;
		}
	}
	#endif
	sprintf(str, "WriteFile_%s_START", root_path);
	WriteLogBuffer(str);
	int writesize = 0;
	for (size_t i = 0; i < WRITE_SIZE; )
	{
		buffer[0] = i;
		int ofs = std::rand();
		ofs = ofs & ((FILE_SIZE/4096)-1);
		ofs *= 4096;
		assert(lseek(fd, ofs, SEEK_SET) == ofs);

		writesize = write(fd, buffer, 4096);
		if(writesize > 0) i+= writesize;
		else printf("write error %d\n", errno);
	}
	
	close(fd);
	sprintf(str, "WriteFile_%s_END", root_path);
	WriteLogBuffer(str);

	fd = open(filename, O_RDONLY, 0100644); // | O_SYNC | O_DIRECT
	if (-1 == fd)
	{
		printf("open file errno %d\n", errno);;
		return FALSE;
	}

	sprintf(str, "ReadFile_%s_START", root_path);
	WriteLogBuffer(str);
	int readsize = 0;
	for (size_t i = 0; i < WRITE_SIZE; )
	{
		int ofs = std::rand();
		ofs = ofs & ((FILE_SIZE/4096)-1);
		ofs *= 4096;
		assert(lseek(fd, ofs, SEEK_SET) == ofs);
		readsize= read(fd, buffer, 4096);
		if(readsize>0) i+=readsize;
		else printf("read error %d\n", errno);
	}

	close(fd);
	sprintf(str, "ReadFile_%s_END", root_path);
	WriteLogBuffer(str);
#else
	char str[128];
	WCHAR filename[128];
	memset(str, 0x00, sizeof(str));
	strcat(str, root_path);
	strcat(str, CPP_FILE_NAME);
	std::string openName(str);

	memset(filename, 0, sizeof(WCHAR) * ((openName.length() + 1)));

#ifdef __linux__
	mbstou16cs(filename, (const char*)openName.c_str(), openName.length());

#else
	mbstowcs(filename, (const char*)openName.c_str(), openName.length());
#endif

	OVERLAPPED ov;
	int ofs;
	memset(&ov, 0x00, sizeof(OVERLAPPED));

	HANDLE fp = CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp != INVALID_HANDLE_VALUE)
	{
		unsigned long setSize = 0;

		sprintf(str, "WriteFile_%s_START", root_path);
		WriteLogBuffer(str);
		unsigned int i;
		for (i = 0;i < WRITE_SIZE;)
		{
			ofs = std::rand();
			ofs = ofs & ((FILE_SIZE/4096)-1);
			ofs *= 4096;
			ov.Offset = ofs;
			if (TRUE == WriteFile(fp, buffer, 4096, &setSize, &ov))
			{
				i += setSize;
			}
			else
			{
				printf("write error");
			}
		}
		printf("WriteSize=%d", i);
		CloseHandle(fp);
		sprintf(str, "WriteFile_%s_END", root_path);
		WriteLogBuffer(str);



	}

	fp = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING
	if (fp != INVALID_HANDLE_VALUE)
	{
		unsigned long setSize = 0;
		sprintf(str, "ReadFile_%s_START", root_path);
		WriteLogBuffer(str);
		unsigned int i;
		for (i = 0; i < WRITE_SIZE;)
		{
			buffer[0] = i;

			ofs = std::rand();
			ofs = ofs & ((FILE_SIZE/4096)-1);
			ofs *= 4096;
			ov.Offset = ofs;

			if (TRUE == ReadFile(fp, buffer, 4096, &setSize, &ov))
			{
				i += setSize;
			}
			else
			{
				printf("read error");
			}

		}
		printf("ReadSize=%d", i);
		CloseHandle(fp);

		sprintf(str, "ReadFile_%s_END", root_path);
		WriteLogBuffer(str);

	}
#endif

	return TRUE;
}
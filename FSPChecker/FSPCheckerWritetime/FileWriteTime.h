#pragma once
#ifndef __FILEWRITETIME_H__
#define __FILEWRITETIME_H__

#include <windows.h>

class FileWriteTime
{
public:
    FileWriteTime();
    ~FileWriteTime();
    static BOOL	FileWrite_hundred(int count);
    static BOOL	FileWrite_rand();
};

#endif
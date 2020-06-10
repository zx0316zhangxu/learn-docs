#include "FileWriteTime.h"
#include <tchar.h>
#include <iostream>
#include <time.h>

#define WRITE_SIZE (1024 * 10)
#define FILE_SIZE (1024 * 1024 * 100)
long buffer[4096 / 4];
unsigned long setSize = 0;
BOOL FileWriteTime::FileWrite_hundred(int count)
{
    HANDLE hfile = NULL;
    for (int i = 0; i < count; i++)
    {
        WCHAR filename[128];
        WCHAR FULLPATH[256];
        swprintf(filename,100,_T("%d.txt"),i);
        swprintf(FULLPATH,100, _T("%s%s"),_T("F:\\Zhangxu\\TestFileWrie\\"),filename);
        hfile = CreateFile(FULLPATH, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hfile == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }
        for (int j = 0; j < WRITE_SIZE; j++)
        {
           buffer[0] = j;
           BOOL ret = WriteFile(hfile, buffer, 1024, &setSize, NULL);
           if (ret == 0)
           {
               CloseHandle(hfile);
               return FALSE;
           }
        }  
        CloseHandle(hfile);
    } 
    return TRUE;
}
BOOL FileWriteTime::FileWrite_rand()
{
    int i = 0;
    int j = 0;
    HANDLE hfile = NULL;
    bool flag = true;
    int a[20] = { 0 };
    WCHAR filename[128];
    WCHAR FULLPATH[256];
    OVERLAPPED ov;
    memset(&ov, 0x00, sizeof(OVERLAPPED)); 
    srand(time(NULL));
    clock_t start, end;
    start = clock();
    for (i = 0; i < 20; i++)
    {
        flag = true;
        j = rand() % 100;
        a[i] = j;
        for (int n = 0; n < i; n++)
        {  
            if (a[n] == j)
            {
                i--;
                flag = false;
            }
        }
        if (flag == true)
        {
            swprintf(filename, 100, _T("%d.txt"), j);
            swprintf(FULLPATH, 100, _T("%s%s"), _T("F:\\Zhangxu\\TestFileWrie\\"), filename);
            hfile = CreateFile(FULLPATH, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hfile == INVALID_HANDLE_VALUE)
            {
                return FALSE;
            }
           
            for (int k = 0; k < 1024; k++)
            {
                ov.Offset = 1024*1024*10 + k*1024;
                //ov.Offset = GetFileSize(hfile, 0) + k * 1024;
                buffer[0] = k;
                BOOL ret = WriteFile(hfile, buffer, 1024, &setSize, &ov);
                FlushFileBuffers(hfile);
                if (ret == 0)
                {
                    CloseHandle(hfile);
                    return FALSE;
                }
            }
            CloseHandle(hfile);
        }  
    }
    end = clock();
    double result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("%f\n", result);
    printf("%ld\n", start);
    printf("%ld\n", end);
    return TRUE;
}
#include "FileWriteTime.h"
#include <iostream>

int main()
{
    BOOL ret;
    // ret = FileWriteTime::FileWrite_hundred(100);
     ret = FileWriteTime::FileWrite_rand();
    if (ret == FALSE)
    {
        std::cout << "error!\n";
    }
    else
    { 
        std::cout << "success!\n";
    }
    return 0;
}
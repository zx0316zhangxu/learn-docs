#pragma once

#if !defined(__FILE_WRITEREAD__)
#define __FILE_WRITEREAD__

#include "windows.h"

/*!
	@brief	ファイル入出力クラス
*/
class FileWriteRead
{
public:
	FileWriteRead(void);						//コンストラクタ
	virtual ~FileWriteRead(void);				//デストラクタ

	//FileIO
	static BOOL	Excute(const char* root_path);

private:

};
#endif		//__FILE_WRITEREAD__


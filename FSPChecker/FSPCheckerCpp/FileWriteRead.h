#pragma once

#if !defined(__FILE_WRITEREAD__)
#define __FILE_WRITEREAD__

#include "windows.h"

/*!
	@brief	�t�@�C�����o�̓N���X
*/
class FileWriteRead
{
public:
	FileWriteRead(void);						//�R���X�g���N�^
	virtual ~FileWriteRead(void);				//�f�X�g���N�^

	//FileIO
	static BOOL	Excute(const char* root_path);

private:

};
#endif		//__FILE_WRITEREAD__


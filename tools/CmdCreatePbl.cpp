#include "stdafx.h"

int CmdCreatePbl(char* pblfile,char* comments /*= NULL*/)
{
	if (pblfile == NULL || strlen(pblfile) == 0)
	{
		return -1;
	}

	if(::PathFileExistsA(pblfile))
	{
		printf("  [ERROR] pblfile existed.\n");
		return false;
	}

	int result = PBORCA_OK;

	HPBORCA hp = PBORCA_SessionOpen();

	result = PBORCA_LibraryCreate(hp,pblfile,(comments != NULL) ? comments : "");

	if (result != PBORCA_OK)
	{
		printf("  [ERROR] Create library error, code = %d\n",result);
		PBORCA_SessionClose(hp);
		return -1;
	}
	
	printf(" %s [created].\n",pblfile);

	PBORCA_SessionClose(hp);

// 	_PBLLIST lstPbl;
// 	DWORD dwAttr = 0;
// 
// 	dwAttr = ::GetFileAttributes(srcpath);
// 	if (dwAttr == (DWORD)-1)
// 	{
// 		cout << "Source file or directory is not exsit!" << endl;
// 		return -1;
// 	}
// 	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
// 	{
// 		char& _rear = *(srcpath + strlen(srcpath) - 1);
// 		if (_rear == '\\')
// 		{
// 			_rear = 0;
// 		}
// 		lstPbl = GetPblFilesFromDirectory(srcpath);
// 	}
// 	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
// 	{
// 		lstPbl.push_back(srcpath);
// 	}
// 
// 	dwAttr = ::GetFileAttributes(dstpath);
// 	if (dwAttr == (DWORD)-1)
// 	{
// 		::CreateDirectoryA(dstpath,NULL);
// 	}
// 	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
// 	{
// 		cout << "Destination path cannot be a file." << endl;
// 		return -1;
// 	}
// 
// 	size_t lstsize = lstPbl.size();
// 	for (size_t i = 0; i < lstsize; i++)
// 	{
// 		DoExport(lstPbl[i],dstpath);
// 	}

	return 0;
}
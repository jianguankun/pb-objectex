#include "stdafx.h"
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Utils.h"

//rebuild ..\terminal-project\terminal.pbt

void CALLBACK rebuild_comperr_callback(PPBORCA_COMPERR _err,LPVOID a)
{
	if (_err->lpszMessageText)
	{
// 		std::string* _s = (std::string*)a;
// 		_s->append(_err->lpszMessageText);
// 		_s->append("\n");
		cout << _err->lpszMessageText << endl;
	}
	return;
}

int CmdRebuildApplication(char* pbtfile, char* buildmode)
{
	if (pbtfile == NULL || strlen(pbtfile) == 0)
	{
		return -1;
	}

	char pbtpath[MAX_PATH] = {NULL};
	strcpy_s(pbtpath,pbtfile);
	::PathRemoveFileSpecA(pbtpath);

	//从pbtfile提取pbt信息，dstfile导入entry时需要pbt信息以及辅助编译
	TPbtInfo pbtInfo;
	if (!AnalysePbtInfo(pbtInfo,pbtfile,pbtpath))
	{
		return -1;
	}

	size_t numsOfPbl = pbtInfo.lstPbl.size();
	if (numsOfPbl == 0)
	{
		cout << "error: No pbl in pbt file." << endl;
		return -1;
	}

	LPTSTR* _pbllist = NULL;
	HPBORCA hp = NULL;
	int ret = 0;

	try
	{
		//把vector形式的PblList转成字符串数组形式,便于PBORCA_SessionSetLibraryList函数使用
		_pbllist = TransferPblList(pbtInfo);
		if (_pbllist == NULL)
		{
			throw g_szErrorMsg;
		}

		int result = PBORCA_OK;
		hp = PBORCA_SessionOpen();

		result = PBORCA_SessionSetLibraryList(hp,_pbllist,numsOfPbl);
		if (result != PBORCA_OK)
		{
			PBORCA_SessionGetError(hp,g_szErrorMsg,PBORCA_MSGBUFFER);
			throw g_szErrorMsg;
		}

		result = PBORCA_SessionSetCurrentAppl(hp,(char*)pbtInfo.applib.c_str(),(char*)pbtInfo.appname.c_str());
		if (result != PBORCA_OK)
		{
			PBORCA_SessionGetError(hp,g_szErrorMsg,PBORCA_MSGBUFFER);
			throw g_szErrorMsg;
		}

		pborca_rebuild_type type;
		if (_stricmp(buildmode,"INC") == 0)
		{
			type = PBORCA_INCREMENTAL_REBUILD;
		}
		else if (_stricmp(buildmode,"MIG") == 0)
		{
			type = PBORCA_MIGRATE;
		}
		else
		{
			type = PBORCA_FULL_REBUILD;
		}

		result = PBORCA_ApplicationRebuild(hp, type,rebuild_comperr_callback,0);
		if (result != PBORCA_OK)
		{
			PBORCA_SessionGetError(hp,g_szErrorMsg,PBORCA_MSGBUFFER);
			throw g_szErrorMsg;
		}
	}
	catch(char* e)
	{
		ret = -1;
		printf("%s\n",e);
	}

	if (_pbllist != NULL)
	{
		delete[] _pbllist;
		_pbllist = NULL;
	}

	if (hp != NULL)
	{
		PBORCA_SessionClose(hp);
	}

	return 0;
}
// Sha1Tester.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	if (argc == 1)
	{
		return 0;
	}

	//��ȡ������ǰ��Ŀ¼
	::GetModuleFileNameA(NULL,g_szAppPath,MAX_PATH);
	::PathRemoveFileSpecA(g_szAppPath);

	//��ȡ����Ŀ¼·��
	::GetCurrentDirectoryA(MAX_PATH,g_szCurrPath);

	if (strcmp(argv[1],"export") == 0)
	{
		if (argc != 4)
		{
			cout << "Number of error operators." << endl;
			return 0;
		}
		CmdExportPbl(argv[2],argv[3]);
	}
	else if (strcmp(argv[1],"import") == 0)
	{
		if (argc != 5)
		{
			cout << "Number of error operators." << endl;
			return 0;
		}
		CmdImportPbl(argv[2],argv[3],argv[4]);
	}
	else if (strcmp(argv[1],"rebuild") == 0)
	{
		if (argc == 3)
		{
			CmdRebuildApplication(argv[2],"full");
		}
		else if(argc == 4)
		{
			CmdRebuildApplication(argv[2],argv[3]);
		}
		else
		{
			cout << "Number of error operators." << endl;
		}
		return 0;
	}
	else
	{
		cout << "Error subcommand." << endl;
	}

	return 0;
}


/**
 * �Զ�����쳣������
 *
 * @param pExceptionInfo ���յ����쳣����ָ��
 *
 * @return ���ش�����
 */
LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("pbtools exit with fatal error! 0x%p\n",pExceptionInfo->ExceptionRecord->ExceptionCode);
    exit( pExceptionInfo->ExceptionRecord->ExceptionCode); 
	return EXCEPTION_EXECUTE_HANDLER;
}
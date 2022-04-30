// Sha1Tester.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;

void ShowHelp();

int _tmain(int argc, _TCHAR* argv[])
{
	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	if (argc == 1)
	{
		ShowHelp();
		return 0;
	}

	//获取主程序当前的目录
	::GetModuleFileNameA(NULL,g_szAppPath,MAX_PATH);
	::PathRemoveFileSpecA(g_szAppPath);

	//获取工作目录路径
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
 * 自定义的异常处理函数
 *
 * @param pExceptionInfo 接收到的异常处理指针
 *
 * @return 返回处理结果
 */
LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("pbtools exit with fatal error! 0x%p\n",pExceptionInfo->ExceptionRecord->ExceptionCode);
    exit( pExceptionInfo->ExceptionRecord->ExceptionCode); 
	return EXCEPTION_EXECUTE_HANDLER;
}

void ShowHelp()
{
	string s = "";
	s += "pbex 1.0\n"; 
	s += "Help: https://gitee.com/jianguankun/pb-objectex\n"; 
	s += "\n";
	s += "  PowerBuilder project is generally composed of PBL and PBT files.\n"; 
	s += "  Form objects, function objects, data containers and other Pb objects\n"; 
	s += "  in the project are stored in PBL files. PowerBuilder ide can export\n"; 
	s += "  Pb objects in PBL files into independent object files, but the function\n"; 
	s += "  of batch export is not very friendly. The tool provided by PowerBuilder\n"; 
	s += "  objectex can export PBL files to Pb objects in batches. In addition to\n"; 
	s += "  batch export, you can also batch import objects into PBL files.\n"; 
	s += "\n";
	s += "Usage: pbex <command>";
	s += "\n";
	s += "\n";
	s += "where <command> is one of:";
	s += "\n";
	s += "\n";
	s += "  export      Export all Pb object files from the specified PBL file.\n"; 
	s += "              Usage: pbex export <src> <dst>\n"; 
	s += "              <src> - PBL file or pbl files's directory\n"; 
	s += "              <dst> - the target directory that pb objects exported.\n"; 
	s += "\n";
	s += "  import      Import the Pb object file into the specified PBL file.\n"; 
	s += "              Usage: pbex import <src> <pbtfile> <dst>\n"; 
	s += "              <src>     - PB objects files directory.\n"; 
	s += "              <pbtfile> - PBT file.When compiling to PBL file, it is used to \n"; 
	s += "                          import objects into PBL file\n"; 
	s += "              <dst> -     target PBL file imported. \n"; 
	s += "\n";
	s += "  rebuild     Compile the specified PBT file.\n"; 
	s += "              Usage: pbex rebuild <pbtfile> [mode]\n"; 
	s += "              <pbtfile> - PBT file.\n"; 
	s += "              [mode] - Build mode, \"FULL\"(default) full build\n"; 
	s += "                      \"INC\"(default) Incremental build\n"; 
	s += "                      \"MIG\"(default) Migrate build\n"; 
	s += "\n";
	cout << s;
}
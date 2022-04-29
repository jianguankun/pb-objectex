// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#include <stdio.h>
#include <tchar.h>
#include <iostream>
using namespace std;
#include <string.h>
#include <io.h>
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include <atlbase.h>
#include <atlstr.h>

#define TIXML_USE_STL

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "STL_def.h"
//#include "BzDataTypes.h"

//https://www.docin.com/p-586788199.html
#include "pborca/pborca.h"

// MS VC++ 11.0_MSC_VER = 1700 (Visual C++ 2012)
// MS VC++ 10.0 _MSC_VER = 1600
// MS VC++ 9.0 _MSC_VER = 1500
// MS VC++ 8.0 _MSC_VER = 1400
// MS VC++ 7.1 _MSC_VER = 1310
// MS VC++ 7.0 _MSC_VER = 1300
// MS VC++ 6.0 _MSC_VER = 1200
// MS VC++ 5.0 _MSC_VER = 1100

#if defined(_MSC_VER) && _MSC_VER == 1600   
#define LIBPATH_PUBLIC(f) "../../_bzpublic_sdk/lib_vc10/"##f
#define LIBPATH_INNER(f) "../../_inner_sdk/lib_vc10/"##f
#endif   

#if defined(_MSC_VER) && _MSC_VER == 1400   
#define LIBPATH_PUBLIC(f) "../../_bzpublic_sdk/lib_vc8/"##f
#define LIBPATH_INNER(f) "../../_inner_sdk/lib_vc8/"##f
#endif

//����BzCommonͨ�ÿ��sdk
//#include "bz_common/BzCommon.h"
#if _DEBUG
//#pragma comment(lib,LIBPATH_PUBLIC("bz_common/BzCommon_static_d.lib"))
#pragma comment(lib,LIBPATH_PUBLIC("pborca/pborca.lib"))
#else
//#pragma comment(lib,LIBPATH_PUBLIC("bz_common/BzCommon_static.lib"))
#pragma comment(lib,LIBPATH_PUBLIC("pborca/pborca.lib"))
#endif

extern char g_szAppPath[MAX_PATH];
extern char g_szCurrPath[MAX_PATH];
extern char g_szErrorMsg[PBORCA_MSGBUFFER];


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
int CmdExportPbl(char* srcpath,char* dstpath);
int CmdImportPbl(char* srcpath,char* pbtpath,char* dstpath);
int CmdRebuildApplication(char* pbtfile, char* buildmode);
char *trim(char *str);

//��ʱ��time_tת���ַ���
std::string tm_2_str(SYSTEMTIME& tm);
std::string tm_2_str2(SYSTEMTIME& tm);

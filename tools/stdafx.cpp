// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Sha1Tester.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������


char g_szAppPath[MAX_PATH] = {NULL};
char g_szCurrPath[MAX_PATH] = {NULL};
char g_szErrorMsg[PBORCA_MSGBUFFER] = {NULL};

char *trim(char *str)
{
	char *p = str;
	char *p1;
	if(p)
	{
		p1 = p + strlen(str) - 1;
		while(*p && isspace(*p)) p++;
		while(p1 > p && isspace(*p1)) *p1-- = '\0';
	}
	return p;
}

std::string tm_2_str(SYSTEMTIME& tm) 
{
	char szBuf[64] = {0};
	sprintf_s(szBuf,"%04d/%02d/%02d %02d:%02d:%02d",
		tm.wYear, tm.wMonth, tm.wDay,
		tm.wHour, tm.wMinute, tm.wSecond);
	std::string s = szBuf;
	return s;
}

std::string tm_2_str2(SYSTEMTIME& tm) 
{
	char szBuf[64] = {0};
	sprintf_s(szBuf,"%04d%02d%02d-%02d%02d%02d",
		tm.wYear, tm.wMonth, tm.wDay,
		tm.wHour, tm.wMinute, tm.wSecond);
	std::string s = szBuf;
	return s;
}
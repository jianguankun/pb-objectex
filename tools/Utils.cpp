#include "stdafx.h"
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Utils.h"

//分析pbt函数
bool AnalysePbtInfo(__out TPbtInfo& _info,char* pbtfile,char* pblpath)
{
	_info.appname = "";
	_info.applib = "";
	_info.lstPbl.clear();

	if(!::PathFileExistsA(pbtfile))
	{
		printf("AnalysePbt: pbt file not found.\n");
		return false;
	}

	std::ifstream infile(pbtfile,ios::binary);
	if (!infile.is_open())
	{
		printf("AnalysePbt: Open pbt file fail.\n");
		return false;
	}

	infile.seekg (0, infile.end);
	int length = (int)infile.tellg() + 2;
	infile.seekg (0, infile.beg);

	if (length > 100000)
	{
		printf("AnalysePbt: It doesn't look like legal pbt file.\n");
		infile.close();
		return false;
	}

	char* buffer = (char*)::calloc(1,length);
	if (buffer == NULL)
	{
		printf("AnalysePbt: alloc memory fail, size = %d.\n",length);
		infile.close();
		return false;
	}

	#define removequo(s) {\
	if(*s == '\"') s++;\
	char& _c = *(s + strlen(s) - 1);\
	if(_c == '\"') _c = '\0';}

	while(!infile.eof())
	{
		infile.getline(buffer,length);
		char* _next = NULL;
		char* _word = ::strtok_s(buffer," \"",&_next);
		if (_word != NULL)
		{
			if(_stricmp(_word,"appname") == 0)
			{
				char* _item = ::strtok_s(NULL,"\"",&_next);
				removequo(_item);
				_info.appname = _item;
			}
			else if(_stricmp(_word,"applib") == 0)
			{
				char* _item = ::strtok_s(NULL,"\"",&_next);
				removequo(_item);
				_info.applib = pblpath;
				_info.applib += "\\";
				_info.applib += _item;
				char _rpath[MAX_PATH] = {NULL};
				::PathCombineA(_rpath,g_szCurrPath,_info.applib.c_str());
				_info.applib = _rpath;

			}
			else if(_stricmp(_word,"LibList") == 0)
			{
				char* _item = ::strtok_s(NULL,"\"",&_next);
				removequo(_item);
				char* _pbl = ::strtok_s(_item,";",&_next);
				while(_pbl != NULL)
				{
					std::string s = "";
					s += pblpath;
					s += "\\";
					s += _pbl;
					char _rpath[MAX_PATH] = {NULL};
					::PathCombineA(_rpath,g_szCurrPath,s.c_str());
					_info.lstPbl.push_back(_rpath);
					_pbl = ::strtok_s(NULL,";",&_next);
				}
			}
		}
	}

	infile.close();
	free(buffer);

	return true;
}

//把vector形式的PblList转成字符串数组形式
LPTSTR* TransferPblList(TPbtInfo& _info)
{
	g_szErrorMsg[0] = '\0';

	size_t numsOfPbl = _info.lstPbl.size();
	if (numsOfPbl == 0)
	{
		sprintf_s(g_szErrorMsg,"error: number of pbl is 0.");
		return NULL;
	}

	LPTSTR* _pbllist = new LPTSTR[numsOfPbl];
	if (_pbllist == NULL)
	{
		sprintf_s(g_szErrorMsg,"error: alloc memory fail.");
		return NULL;
	}

	for (size_t i = 0; i < numsOfPbl; ++i)
	{
		_pbllist[i] = (LPTSTR)_info.lstPbl[i].c_str();
	}

	return _pbllist;
}

//分解字符串
std::vector<std::string> split_str(const std::string& s, char delimiter, int nhead)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	int headcount = 0;
	while ( std::getline(tokenStream, token, delimiter) )
	{
		headcount++;
		tokens.push_back(token);
		if (nhead > 0 && headcount == nhead)
		{
			if(!tokenStream.eof())
			{
				std::getline(tokenStream, token);
				tokens.push_back(token);
			}
			break;
		}
	}
	return tokens;
}
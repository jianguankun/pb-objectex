/**********************************************************************************
	created:	2014/07/29
	author:		KFGame.com
	
	purpose:	标准库常用定义
**********************************************************************************/

#ifndef __STL_DEF_HEADERFILE__
#define __STL_DEF_HEADERFILE__

#include <map>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

template<typename T> 
struct XIDComparer
{
	bool operator()(const T& x,const T& y) const
	{
		return (memcmp(&x,&y,sizeof(T)) < 0);
	}
};

struct StrComparer
{
	bool operator()(const tstring& x,const tstring& y) const
	{
		return (x.compare(y) < 0);
	}
};

//dll模块表
extern std::map<GUID,HMODULE,XIDComparer<GUID>> g_MuduleMap;

#endif //__STL_DEF_HEADERFILE__

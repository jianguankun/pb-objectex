#include "stdafx.h"
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

//https://www.docin.com/p-586788199.html
#include "pborca/pborca.h"

typedef std::vector<std::string> _PBLLIST;

struct TPbtInfo
{
	std::string appname;
	std::string applib;
	_PBLLIST    lstPbl;
};

//����pbt����
bool AnalysePbtInfo(__out TPbtInfo& _info,char* pbtfile,char* pblpath);

//��vector��ʽ��PblListת���ַ���������ʽ
LPTSTR* TransferPblList(TPbtInfo& _info);
#include "stdafx.h"
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Utils.h"

//import icclient-export\icclient.pbl icclient-project\icclient.pbt icclient-project\icclient.pbl
//import terminal-export\terminal.pbl terminal-project\terminal.pbt terminal-project\terminal.pbl

typedef std::vector<std::string> _PBLLIST;

struct TPblEntryInfo
{
	std::string    comments;
	LONG           lCreateTime;                /* Time of entry create/mod */ 
	LONG           lEntrySize;                 /* Size of entry */
	std::string    entryName;              /* Pointer to entry name */
	PBORCA_TYPE    otEntryType;                /* Entry type */
	LONG           lObjectSize;                /* Size of object */
	LONG           lSourceSize;                /* Size of source */
};

struct TEntryNode
{
	PBORCA_TYPE type;
	std::string entryname;
	std::string file;
	unsigned int filesize;
};

struct TEntrySummary
{
	unsigned int nMaxFileSize;
	unsigned int uFileNameLen;
};

typedef std::vector<TEntryNode> _ENTRYLIST;

//初始化文件日志
bool InitImportLog(__out std::ofstream& logger, __out std::string& logfile, char* dstpblfile);
void CloseImportLog(__out std::ofstream& logger);

//备份pbl
BOOL BackupPbl( __out std::string& bakfile,char* pblfile);

//检查目标pblfile的合法性
bool CheckDstPblFile(TPbtInfo& _info,char* dstfile,__out int& index);

//从一个目录中获取所有entry信息
bool GetEntriesFromDirectory(__out _ENTRYLIST& entrylst, __out TEntrySummary& summary, std::string path);

void CALLBACK pbcallback(PPBORCA_COMPERR _err,LPVOID a)
{
	if (_err->lpszMessageText)
	{
		std::string* _s = (std::string*)a;
		_s->append(_err->lpszMessageText);
		_s->append("\n");
	}
	return;
}

//https://docs.appeon.com/appeon_online_help/pb2017r3/orca_guide/ch02s07.html
int CmdImportPbl(char* srcpath, char* pbtfile, char* dstfile)
{
	if (srcpath == NULL || strlen(srcpath) == 0 
		|| pbtfile == NULL || strlen(pbtfile) == 0
		|| dstfile == NULL || strlen(dstfile) == 0)
	{
		return -1;
	}

	//获取目标文件路径
	char dstpath[MAX_PATH] = {NULL};
	::PathCombineA(dstpath,g_szCurrPath,dstfile);
	::PathRemoveFileSpecA(dstpath);

	DWORD dwAttr = 0;

	//判断源文件对象的目录srcpath合法性
	dwAttr = ::GetFileAttributes(srcpath);
	if (dwAttr == (DWORD)-1)
	{
		cout << "Source file or directory is not exsit!" << endl;
		return -1;
	}
	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		char& _c = *(srcpath + strlen(srcpath) - 1);
		if (_c == '\\' || _c == '/')
		{
			_c = '\0';
		}
	}
	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
	{
		cout << "error: Source must be a directory." << endl;
		return -1;
	}
	 
	//从pbtfile提取pbt信息，dstfile导入entry时需要pbt信息以及辅助编译
	TPbtInfo pbtInfo;
	if (!AnalysePbtInfo(pbtInfo,pbtfile,dstpath))
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
	char* pReadBuf = NULL;
	HPBORCA hp = NULL;
	int ret = 0;
	std::ofstream logger;
	std::string logfile;

	try
	{
		int idx_dstpbl = 0;
		std::string errstr;
		
		//检查目标pbl的合法性
		if (!CheckDstPblFile(pbtInfo,dstfile,idx_dstpbl))
		{
			throw g_szErrorMsg;
		}

		//把vector形式的PblList转成字符串数组形式,便于PBORCA_SessionSetLibraryList函数使用
		_pbllist = TransferPblList(pbtInfo);
		if (_pbllist == NULL)
		{
			throw g_szErrorMsg;
		}

		_ENTRYLIST lstEntries;
		TEntrySummary summary;
		if (!GetEntriesFromDirectory(lstEntries,summary,srcpath))
		{
			throw g_szErrorMsg;
		}

		//读Entry文件内存，申请到最大，一次申请即可
		pReadBuf = (char*)::malloc(summary.nMaxFileSize);
		if (NULL == pReadBuf)
		{
			throw "Alloc read buffer memory fail.\n";
		}

		std::string bakpblfile;
		if (!BackupPbl(bakpblfile,dstfile))
		{
			throw "Backup pbl fail.";
		}

		//初始化日志
		if (!InitImportLog(logger,logfile,dstfile))
		{
			throw "Init import logger fail.";
		}

		int result = PBORCA_OK;
		hp = PBORCA_SessionOpen();

	// 	result = PBORCA_LibraryDelete(hp, pbllst[1]);   
	// 	result = PBORCA_LibraryCreate(hp,pbllst[1],"Test Create");
	
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

		//导入逻辑
		std::string sComplieOutput = "";
		size_t lstsize = lstEntries.size();
		int successCnt = 0,comperrCnt = 0,failCnt = 0;
		std::stringstream ss;
		ss.flags(ios::left); 

		for (size_t i = 0; i < lstsize; i++)
		{
			TEntryNode& _entry = lstEntries[i];
			std::ifstream infile(_entry.file,ios::binary);

			infile.read(pReadBuf,_entry.filesize);
			infile.close();

			ss << "Import " << std::setw(summary.uFileNameLen + 2) << std::setfill(' ') << _entry.file.c_str();
			cout << ss.str();logger << ss.str();
			ss.clear();ss.str("");
			result = PBORCA_CompileEntryImport(hp,_pbllist[idx_dstpbl],(char*)_entry.entryname.c_str(),_entry.type,"",
												pReadBuf,_entry.filesize,pbcallback,(LPVOID)&sComplieOutput);
			if (result == PBORCA_OK)
			{
				successCnt++;
				ss << "  [Success]" << endl;
			}
			else if (result == PBORCA_COMPERROR)
			{
				comperrCnt++;
				ss << "  [Success], but complie error." << endl;
			}
			else
			{
				failCnt++;
				PBORCA_SessionGetError(hp,g_szErrorMsg,PBORCA_MSGBUFFER);
				ss << endl << "  " << result << " " << g_szErrorMsg << endl;
			}
			cout << ss.str();logger << ss.str();
			ss.clear();ss.str("");
			infile.close();
		}

		cout << endl;logger << endl;
		ss << "Import " << lstsize << " entries to " << dstfile << "." << endl;
		cout << ss.str();logger << ss.str();
		ss.clear();ss.str("");
		ss << successCnt << " Success, " << comperrCnt << " Complie error, " << failCnt << " fail." << endl;
		cout << ss.str();logger << ss.str();
		ss.clear();ss.str("");
		ss << "Pbl file backup to:" << endl << "  " << bakpblfile << endl;
		cout << ss.str();logger << ss.str();
		cout << "Log recorded at:" << endl << "  " << logfile << endl;
		if (comperrCnt > 0 && sComplieOutput.length() > 0)
		{
			logger << endl;
			logger << "Complie error messge:" << endl << endl;
			logger << sComplieOutput << endl;
		}
		cout << endl;
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

	if (pReadBuf != NULL)
	{
		delete[] pReadBuf;
		pReadBuf = NULL;
	}

	if (hp != NULL)
	{
		PBORCA_SessionClose(hp);
	}

	CloseImportLog(logger);

	return ret;
}

//初始化文件日志
bool InitImportLog(__out std::ofstream& logger,__out std::string& logfile, char* dstpblfile)
{
	char outputfile[MAX_PATH] = {NULL};
	char name[MAX_PATH] = {NULL};
	strcpy_s(outputfile,dstpblfile);
	::PathRemoveFileSpecA(outputfile);
	::PathAppendA(outputfile,"import-logs");

	DWORD dwAttr = ::GetFileAttributes(outputfile);
	if (dwAttr == (DWORD)-1)
	{
		::CreateDirectoryA(outputfile,NULL);
	}

	char* _ext = ::PathFindExtensionA(dstpblfile);
	if (_ext != NULL)
	{
		memcpy(name,dstpblfile,_ext - dstpblfile);
	}
	sprintf_s(name,"%s.log",::PathFindFileNameA(name));
	::PathAppendA(outputfile,name);

	logfile = outputfile;

	logger.open(outputfile,ios::app);
	if (logger.is_open())
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		logger << "[" << ::tm_2_str(st) << "]" << endl;
		return true;
	}

	return false;
}

void CloseImportLog(__out std::ofstream& logger)
{
	if (logger.is_open())
	{
		logger << endl;
		logger.flush();
		logger.close();
	}
}

//备份pbl
BOOL BackupPbl( __out std::string& bakfile,char* pblfile)
{
	char backupfile[MAX_PATH] = {NULL};
	char name[MAX_PATH] = {NULL};
	strcpy_s(backupfile,pblfile);
	::PathRemoveFileSpecA(backupfile);
	::PathAppendA(backupfile,"import-backup");
	DWORD dwAttr = ::GetFileAttributes(backupfile);
	if (dwAttr == (DWORD)-1)
	{
		::CreateDirectoryA(backupfile,NULL);
	}
	SYSTEMTIME st;
	::GetLocalTime(&st);
	sprintf_s(name,"%s.%s",::PathFindFileNameA(pblfile),tm_2_str2(st).c_str());
	::PathAppendA(backupfile,name);
	bakfile = backupfile;

	return ::CopyFileA(pblfile,backupfile,false);
}

//检查目标pblfile的合法性
bool CheckDstPblFile(TPbtInfo& _info,char* dstfile,__out int& index)
{
	index = -1;
	g_szErrorMsg[0] = '\0';

	char fulldstfile[MAX_PATH] = {NULL};
	::PathCombineA(fulldstfile,g_szCurrPath,dstfile);

	DWORD dwAttr = ::GetFileAttributes(dstfile);
	if (dwAttr == (DWORD)-1)
	{
		sprintf_s(g_szErrorMsg,"error: %s not exist.",dstfile);
		return false;
	}
	else if ((dwAttr & FILE_ATTRIBUTE_ARCHIVE) != FILE_ATTRIBUTE_ARCHIVE)
	{
		sprintf_s(g_szErrorMsg,"error: %s is not a file.",dstfile);
		return false;
	}
	
	size_t lstlen = _info.lstPbl.size();
	for (size_t i = 0; i < lstlen; ++i)
	{
		if (strcmp(fulldstfile,_info.lstPbl[i].c_str()) == 0)
		{
			index = i;
		}
	}
	if (-1 == index)
	{
		sprintf_s(g_szErrorMsg,"error: %s not found in pbt file.",dstfile);
		return false;
	}

	return true;
}

//从一个目录中获取所有对象信息
bool GetEntriesFromDirectory(__out _ENTRYLIST& entrylst, __out TEntrySummary& summary, std::string srcpath)
{
	g_szErrorMsg[0] = '\0';
	entrylst.clear();
	summary.nMaxFileSize = 0;
	summary.uFileNameLen = 0;

	std::map<std::string,PBORCA_TYPE> extmap;
	extmap["sra"] = PBORCA_APPLICATION;
	extmap["srd"] = PBORCA_DATAWINDOW ;
	extmap["srf"] = PBORCA_FUNCTION;
	extmap["srm"] = PBORCA_MENU;
	extmap["srq"] = PBORCA_QUERY;
	extmap["srs"] = PBORCA_STRUCTURE;
	extmap["sru"] = PBORCA_USEROBJECT;
	extmap["srw"] = PBORCA_WINDOW;
	extmap["srp"] = PBORCA_PIPELINE;
	extmap["srj"] = PBORCA_PROJECT;
	//extmap["srj"] = PBORCA_PROXYOBJECT;
	extmap["srb"] = PBORCA_BINARY;

	std::string filterstr = srcpath + "\\*.sr*";
	_finddata_t file;
	intptr_t lf;
	if((lf = _findfirst(filterstr.c_str(),&file)) == -1L)
	{
		sprintf_s(g_szErrorMsg,"error: No entry file (*.sr*) in directory.");
		return false;
	}

	int nHandle = lf;
	while(-1L != nHandle)
	{
		if(strcmp(file.name,"..") != 0)
		{
			if(file.attrib & _A_ARCH)
			{
				std::string s =  srcpath + "\\" + file.name;

				TEntryNode node;
				node.file = s;
				char* _ffulname = ::PathFindFileNameA(node.file.c_str());
				char* _ext = ::PathFindExtensionA(node.file.c_str());
				char _fname[MAX_PATH] = {NULL};
				if (_ext != NULL)
				{
					memcpy(_fname,_ffulname,_ext - _ffulname);
					_ext++;
				}
				node.entryname = _fname;

				if (node.file.length() > summary.uFileNameLen)
				{
					summary.uFileNameLen = node.file.length();
				}

				std::ifstream infile(node.file,ios::binary);
				infile.seekg (0, infile.end);
				node.filesize = (unsigned int)infile.tellg();
				if (node.filesize > summary.nMaxFileSize)
				{
					summary.nMaxFileSize = node.filesize;
				}
				infile.close();

				std::map<std::string,PBORCA_TYPE>::iterator _it = extmap.find(_ext);
				node.type = (_it != extmap.end()) ? _it->second : (PBORCA_TYPE)-1;

				entrylst.push_back(node);
			}
		}
		nHandle = _findnext(lf,&file);
	}
	_findclose(lf);

	if (entrylst.size() == 0)
	{
		sprintf_s(g_szErrorMsg,"error: No entry file (*.sr*) in directory.");
		return false;
	}

	return true;
}
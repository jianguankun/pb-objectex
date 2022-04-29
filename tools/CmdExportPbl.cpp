#include "stdafx.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

//export icclient-project\icclient.pbl icclient-export

struct TPblEntryInfo
{
	std::string    comments;
	LONG           lCreateTime;                /* Time of entry create/mod */ 
	LONG           lEntrySize;                 /* Size of entry */
	std::string    entryName;                  /* Pointer to entry name */
	PBORCA_TYPE    otEntryType;                /* Entry type */
	LONG           lObjectSize;                /* Size of object */
	LONG           lSourceSize;                /* Size of source */
	LONG           lBinarySize;                /* Size of binary */

};


typedef std::vector<std::string> _PBLLIST;
typedef std::vector<TPblEntryInfo> _PBO_LIST;

char* g_ExtNames[12] = {
	"sra",  //  PBORCA_APPLICATION
	"srd",  // 	PBORCA_DATAWINDOW 
	"srf",  // 	PBORCA_FUNCTION
	"srm",  // 	PBORCA_MENU
	"srq",  // 	PBORCA_QUERY
	"srs",  // 	PBORCA_STRUCTURE
	"sru",  // 	PBORCA_USEROBJECT
	"srw",  // 	PBORCA_WINDOW
	"srp",  // 	PBORCA_PIPELINE
	"srj",  // 	PBORCA_PROJECT
	"srj",  // 	PBORCA_PROXYOBJECT
	"srb",  // 	PBORCA_BINARY
};

//从一个目录中获取所有pbl的文件名
_PBLLIST GetPblFilesFromDirectory(std::string path);

int DoExport(std::string pblfile,std::string dstpath);

int CmdExportPbl(char* srcpath,char* dstpath)
{
	if (srcpath == NULL || strlen(srcpath) == 0 
		|| dstpath == NULL || strlen(dstpath) == 0)
	{
		return -1;
	}

	_PBLLIST lstPbl;
	DWORD dwAttr = 0;

	dwAttr = ::GetFileAttributes(srcpath);
	if (dwAttr == (DWORD)-1)
	{
		cout << "Source file or directory is not exsit!" << endl;
		return -1;
	}
	else if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		char& _rear = *(srcpath + strlen(srcpath) - 1);
		if (_rear == '\\')
		{
			_rear = 0;
		}
		lstPbl = GetPblFilesFromDirectory(srcpath);
	}
	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
	{
		lstPbl.push_back(srcpath);
	}

	dwAttr = ::GetFileAttributes(dstpath);
	if (dwAttr == (DWORD)-1)
	{
		::CreateDirectoryA(dstpath,NULL);
	}
	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
	{
		cout << "Destination path cannot be a file." << endl;
		return -1;
	}

	size_t lstsize = lstPbl.size();
	for (size_t i = 0; i < lstsize; i++)
	{
		DoExport(lstPbl[i],dstpath);
	}

	return 0;
}

//从一个目录中获取所有pbl的文件名
_PBLLIST GetPblFilesFromDirectory(std::string path)
{
	_PBLLIST v;
	std::string filterstr = path + "\\*.pbl";
	_finddata_t file;
	intptr_t lf;
	if((lf = _findfirst(filterstr.c_str(),&file)) == -1L)
	{
		cout<<"No .pbl file in directory.\n";
		return v;
	}

	int nHandle = lf;
	while(-1L != nHandle)
	{
		if(strcmp(file.name,"..") != 0)
		{
			if(file.attrib & _A_ARCH)
			{
				v.push_back(path + "\\" + file.name);
			}
		}
		nHandle = _findnext(lf,&file);
	}
	_findclose(lf);

	return v;
}



void CALLBACK pbcallback(PPBORCA_DIRENTRY pd,LPVOID a)
{
	_PBO_LIST* _pbo = (_PBO_LIST*)a;
	TPblEntryInfo info;
	info.comments = pd->szComments;
	info.lCreateTime = pd->lCreateTime;
	info.lEntrySize = pd->lEntrySize;
	info.entryName = pd->lpszEntryName;
	info.otEntryType = pd->otEntryType;
	info.lObjectSize = info.lSourceSize = 0;
	_pbo->push_back(info);
}

// int my_PBORCA_LibraryEntryExport ( HPBORCA     hORCASession,
// 	LPTSTR       lpszLibName,  
// 	LPTSTR       lpszEntryName, 
// 	PBORCA_TYPE otEntryType,    
// 	LPTSTR       lpszExportBuffer, 
// 	LONG        lExportBufferSize )
// {
// 	if (strcmp(lpszEntryName,"w_terminaldetection_standard") != 0)
// 	{
// 		return PBORCA_OK;
// 	}
// 	return  lExportBufferSize < 14834 ? PBORCA_BUFFERTOOSMALL : PBORCA_OK;
// }

int DoExport(std::string pblfile,std::string dstpath)
{
	std::string pblname = ::PathFindFileNameA(pblfile.c_str());
	std::string exppath = dstpath + "\\" + pblname;
	printf("Export %s\n",pblfile.c_str());
	DWORD dwAttr = ::GetFileAttributes(exppath.c_str());
	if (dwAttr == (DWORD)-1)
	{
		::CreateDirectoryA(exppath.c_str(),NULL);
	}
	else if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
	{
		printf("  [ERROR] Destination path cannot be a file.\n");
		return -1;
	}

	int result = PBORCA_OK;

	HPBORCA hp = PBORCA_SessionOpen();

	_PBO_LIST lstPBO;
	result = PBORCA_LibraryDirectory(hp,(char*)pblfile.c_str(),"",0,pbcallback,&lstPBO);
	if (result != PBORCA_OK)
	{
		printf("  [ERROR] List library directory error, code = %d\n",result);
		PBORCA_SessionClose(hp);
		return -1;
	}

	//printf("%s, %d objects\n",pblname.c_str(),lstPBO.size());

	//补全TPblEntryInfo数据
	size_t lstsize = lstPBO.size();
	for (size_t i = 0; i < lstsize; ++i)
	{
		TPblEntryInfo& _pbo = lstPBO[i];
		PBORCA_ENTRYINFO t;
		result = PBORCA_LibraryEntryInformation(hp,(char*)pblfile.c_str(),(char*)_pbo.entryName.c_str(),_pbo.otEntryType,&t);
		if (result != PBORCA_OK)
		{
			printf("  [ERROR] %s, Get entry info error, code = %d\n",_pbo.entryName.c_str(),result);
			PBORCA_SessionClose(hp);
			return -1;
		}
		_pbo.lObjectSize = t.lObjectSize;
		_pbo.lSourceSize = t.lSourceSize;
		//PBORCA_LibraryEntryInformation函数使用PBORCA_BINARY函数永远返回-3?
// 		result = PBORCA_LibraryEntryInformation(hp,(char*)pblfile.c_str(),(char*)_pbo.entryName.c_str(),PBORCA_BINARY,&t);
// 		if (result == PBORCA_OK)
// 		{
// 			printf("PBORCA_BINARY.\n");
// 		}
// 		else
// 		{
// 			_pbo.lBinarySize = 0;
// 		}
	}

	//找出pbl中使用存储空间最大的对象，其使用的空间大小赋给lMaxAlloc。
	//在导出pbl所有对象过程中始终使用lMaxAlloc长度的内存空间，减少malloc/free次数
	long lMaxAlloc = 0;
	for (size_t i = 0; i < lstsize; ++i)
	{
		TPblEntryInfo& _pbo = lstPBO[i];
		if (lMaxAlloc < _pbo.lSourceSize)
		{
			lMaxAlloc = _pbo.lSourceSize;
		}
	}
	lMaxAlloc += 1;

	char* expbuf = (char*)::malloc(lMaxAlloc);
	if (NULL == expbuf)
	{
		printf("  [ERROR] Alloc memory fail, size = %d bytes\n",lMaxAlloc);
		PBORCA_SessionClose(hp);
		return -1;
	}

	int iSuccessCnt = 0;
	int iFailCnt = 0;
	
	//把pbl的对象逐一导出到文件
	for (size_t i = 0; i < lstsize; ++i)
	{
		TPblEntryInfo& _pbo = lstPBO[i];

		std::stringstream ssfilename;
		ssfilename << _pbo.entryName << "." << g_ExtNames[_pbo.otEntryType];
		std::stringstream ssfilepath;
		ssfilepath << exppath << "\\" << ssfilename.str();
		std::ofstream outfile(ssfilepath.str(),ios::binary);
		if (!outfile.is_open())
		{
			printf("  [ERROR] %s, Open dst file error.\n",_pbo.entryName.c_str());
			iFailCnt++;
			continue;
		}

		//导出文件头
		outfile << "$PBExportHeader$" << ssfilename.str() << "\r\n";
		if (_pbo.comments.length() > 0)
		{
			outfile << "$PBExportComments$" << _pbo.comments << "\r\n";
		}

		//导出源码内容
		result = PBORCA_LibraryEntryExport(hp,(char*)pblfile.c_str(),(char*)_pbo.entryName.c_str(),_pbo.otEntryType,expbuf,lMaxAlloc);
		if (result != PBORCA_OK)
		{
			printf("  [ERROR] %s, Export entry error, code = %d\n",_pbo.entryName.c_str(),result);
			iFailCnt++;
			continue;
		}
		if (strlen(expbuf) > 0)
		{
			outfile << expbuf;
		}
		
		//导出二进制段
// 		expbuf[0] = '\0';
// 		bool bException = false;
// 		DWORD llMaxAlloc = 100;
// 		while(true)
// 		{
// 			//PBORCA_LibraryEntryExport当PBORCA_BINARY时对llMaxAlloc判断有误，
// 			//如w_terminaldetection_standard对象理应需要14832字节空间，但llMaxAlloc传入12800，函数竟然返回PBORCA_OK，同时函数把expbuf写越界。
//          //本逻辑先不使用
// 			result = PBORCA_LibraryEntryExport(hp,(char*)pblfile.c_str(),(char*)_pbo.entryName.c_str(),PBORCA_BINARY,expbuf,llMaxAlloc);
// 			if (result != PBORCA_OK)
// 			{
// 				if (result == PBORCA_BUFFERTOOSMALL)
// 				{
// 					llMaxAlloc *= 2;
// 					expbuf = (char*)::realloc(expbuf,llMaxAlloc);
// 					if (NULL == expbuf)
// 					{
// 						printf("  [ERROR] %s/%s, Realloc memory fail, size = %d bytes\n",pblname.c_str(),_pbo.entryName.c_str(),llMaxAlloc);
// 						return -1;
// 					}
// 					printf("  [INFO] %s/%s, Realloc memory, size = %d bytes\n",pblname.c_str(),_pbo.entryName.c_str(),llMaxAlloc);
// 					continue;
// 				}
// 				else
// 				{
// 					printf("  [ERROR] %s/%s, Export entry binary error, code = %d\n",pblname.c_str(),_pbo.entryName.c_str(),result);
// 					bException = true;
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 		if (bException)
// 		{
// 			break;
// 		}
		
		outfile.flush();
		outfile.close();
		
		result = PBORCA_LibraryEntryExport(hp,(char*)pblfile.c_str(),(char*)_pbo.entryName.c_str(),PBORCA_BINARY,expbuf,lMaxAlloc);
		if (result != PBORCA_OK)
		{
			int is = 0;
			is = i;
			#ifdef _DEBUG	
				printf("  [ERROR] %s, Export entry binary error, code = %d\n",_pbo.entryName.c_str(),result);
				iFailCnt++;
				continue;
			#else
				//Release版O2优化后PBORCA_LibraryEntryExport常返回-10
				if (result != PBORCA_BUFFERTOOSMALL)
				{
					printf("  [ERROR] %s, Export entry binary error, code = %d\n",_pbo.entryName.c_str(),result);
					iFailCnt++;
					continue;
				}
			#endif // _DEBUG
			
		}

		if (strlen(expbuf) > 0)
		{
			ssfilename.clear();
			ssfilename.str("");
			ssfilename << _pbo.entryName << "." << g_ExtNames[PBORCA_BINARY];
			ssfilepath.clear();
			ssfilepath.str("");
			ssfilepath << exppath << "\\" << ssfilename.str();
			outfile.open(ssfilepath.str(),ios::binary);
			if (!outfile.is_open())
			{
				printf("  [ERROR] %s, Open dst binary file error.\n",_pbo.entryName.c_str());
				iFailCnt++;
				continue;
			}
			//导出文件头
			outfile << "$PBExportHeader$" << ssfilename.str() << "\r\n";
			outfile << expbuf;
			outfile.flush();
			outfile.close();
		}
		iSuccessCnt++;
		
	}

	::free(expbuf);
	expbuf = NULL;

	PBORCA_SessionClose(hp);

	if (iFailCnt == 0)
	{
		printf("  %d Entries, All success.\n",lstPBO.size());
	}
	else
	{
		printf("  %d Entries, %d fail.\n",lstPBO.size(),iFailCnt);
	}
	
	
	return 0;
}
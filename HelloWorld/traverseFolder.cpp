#include "stdafx.h"
#include <iostream>
#include <fstream> 
#include <string>  
#include <io.h>  
#include <string>
#include<stdio.h>
#include<stdlib.h>
#include<direct.h>

#include "traverseFolder.h"
using namespace std;

/*用到数据结构_finddata_t，文件信息结构体的指针。
struct _finddata_t
{
unsigned attrib;     //文件属性
time_t time_create;  //文件创建时间
time_t time_access;  //文件上一次访问时间
time_t time_write;   //文件上一次修改时间
_fsize_t size;  //文件字节数
char name[_MAX_FNAME]; //文件名
};

文件属性是无符号整数，取值为相应的宏：_A_ARCH(存档)，_A_SUBDIR(文件夹)，_A_HIDDEN(隐藏)，_A_SYSTEM(系统)，_A_NORMAL(正常)，_A_RDONLY(只读)。容易看出，通过这个结构体，我们可以得到关于该文件的很多信息。结合以下函数，我们可以将文件信息存储到这个结构体中：

//按FileName命名规则匹配当前目录第一个文件
_findfirst(_In_ const char * FileName, _Out_ struct _finddata64i32_t * _FindData);
//按FileName命名规则匹配当前目录下一个文件
_findnext(_In_ intptr_t _FindHandle, _Out_ struct _finddata64i32_t * _FindData);
//关闭_findfirst返回的文件句柄
_findclose(_In_ intptr_t _FindHandle);*/
/*
	函数作用：查询当前文件夹下的文件
	如果需要指定特定扩展名，需要在文件夹末尾增加扩展名，
	"C:\\Windows\\*.exe"的意思是查询windows目录下扩展名为exe的文件
*/
bool listFiles(string fileName)
{
	_finddata_t fileInfo;
	long handle = _findfirst(fileName.c_str(), &fileInfo);

	if (handle == -1L)
	{
		cerr << "failed to find files" << endl;
		return false;
	}

	do
	{
		if (fileInfo.attrib & _A_SUBDIR) {

		}
		else {
			cout << fileInfo.name << endl;
		}
	} while (_findnext(handle, &fileInfo) == 0);
	_findclose(handle);
	return true;
}


void traverseFolder(string folderPath, ofstream &fout, traverseFolder_handler tf_handler=NULL)
{
	_finddata_t FileInfo;
	//先写死只支持遍历mp4格式的文件
	string strfind = folderPath + "\\*.*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//这个语句很重要
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				traverseFolder(newPath, fout, tf_handler);
				cout << "find new path:"<< newPath << endl;
			}
		}
		else
		{
			if(NULL != tf_handler) {
				string tmpFileName = folderPath + "\\" + FileInfo.name;
				(*tf_handler)(tmpFileName, fout);
			}
			cout << folderPath << "\\" << FileInfo.name << endl;
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	strfind.clear();
	string(strfind).swap(strfind);

	_findclose(Handle);
	//其他函数递归使用fout，这个文件输出流不能关闭，否则会导致其他递归函数写入不进去
	//fout.close();
}
/*
函数作用：根据文件名的绝对路径得到文件夹路径和文件名
*/

int getFolderAndFilename(char* fullPath, char* folder, char* fileName) {

	if (fullPath == NULL || folder == NULL || fileName == NULL) {
		return -1;
	}
	int nLen = strlen(fullPath);
	if (nLen >= 255) {
		printf("路径太长，超过255");
		return -1;
	}
	char chPathTmp[256];
	strcpy(chPathTmp, fullPath);
	if (chPathTmp[nLen - 1] == '\\') {
		//如果最后一个字符是\，直接提示出错，给出的是个文件夹路径
		return -1;
	}
	for (int i = nLen - 1; i >= 0; i--) {
		if (chPathTmp[i] == '\\') {
			//从绝对路径倒序找到一个\，在这之前的都是文件夹路径，在这之后的是文件名
			strcpy(fileName, &chPathTmp[i + 1]);
			chPathTmp[i + 1] = '\0';
			strcpy(folder, chPathTmp);
			break;
		}
	}
	return 0;
}

/*
	函数作用：如果目录不存在，就逐层创建
	例子：如C：\f1\f2\f3\ 只有C：\这个目录的存在，则此代码逐层创建f1，f2,f3三层文件夹
	path最终需要以\\结尾
*/
int mkdirByLevel(const char* path) {
	char *tag;
	char bufPath[256];
	int nLen = strlen(path);
	if (nLen >=255) {
		printf("文件路径长于等于255");
		return -1;
	}
	strcpy(bufPath, path);
	//如果bufPath的结尾不是\，补充一个，这里多加一个字节不会导致越界，前面定义的bufPath长度是256，判断路径
	//最大长度是255, 预留了一个字节
	if (bufPath[nLen - 1] != '\\') {
		bufPath[nLen] = '\\';
		bufPath[nLen+1] = '\0';
	}
	int nPos = 0;
	for (tag = bufPath; *tag; tag++, nPos++) {
		if (*tag == '\\') {
			//找到一个\位置（目录），将当前位置先设为\0，取从路径开始到此文件夹得完整路径bufPath
			char chTmp = bufPath[nPos]; //unicode的情况下这里会有问题
			bufPath[nPos] = '\0';
			////如果文件夹不存在，就创建一个
			if (_access(bufPath, 0) == -1) {
				if (-1 == _mkdir(bufPath)) {
					printf("创建文件夹失败%s", bufPath);
				}
			}
			bufPath[nPos] = chTmp;
		}
	}
	return 0;
}
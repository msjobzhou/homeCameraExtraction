#include "stdafx.h"

#include "video.h"
#include "cv.h"
#include "highgui.h"
#include <string>
#include<stdio.h>
#include<stdlib.h>
#include<direct.h>
#include<io.h>  

#include <vector>

#include "traverseFolder.h"

using namespace std;
/*
函数作用：readvideo 返回视频的帧数（-1时表示视频文件无效, 0表示无帧数），并且保存文件到指定的路径
输入参数：fileName 需要读取视频文件名
		filePath 需要读取视频文件路径，程序假设路径不是以"\"结尾，函数代码中会在文件名前补一个"\"
		period 帧间隔，每隔多少帧取其中一张截图
		saveImgPath 从视频中抓取的图像存放的路径
		vImgPath 把从视频中读取的图像全部保存在vector里，方便后续程序处理
*/
int readVideoSaveImg(const char* fileName, const char* filePath, const char* saveImgPath, int period, vector<string> &vImgPath) {
	//int period;
	int count = 1; //文件编号开始计数值
	
	string fileFullPath = "";
	fileFullPath = fileFullPath + filePath + "\\" + fileName;
	CvCapture *capture = cvCreateFileCapture(fileFullPath.c_str()); //打开视频文件
	//获取视频文件总共有多少帧
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	//视频文件打开为空，直接返回-1
	if (capture == NULL)
	{
		return  -1;
	}
	//printf("totalFrameNum:%d",numFrames);
	IplImage *frame;
	string strSaveImgPath="";
	int nPos = period;
	//从视频中抓取的图像，保存的文件名
	char chFilename[128];
	//while (1)
	while(nPos<= numFrames && cvGrabFrame(capture))
	{
		//逐帧获取的方式改成获取特定帧的方式，提升效率
		/*for (int i = 0; i < period; i++)
		{
			frame = cvQueryFrame(capture);
			//printf("query frame");
			if (!frame)
			{
				//printf("finish!\n");
				//这里没有释放视频资源(函数末尾释放资源的cvReleaseCapture实际上走不到)，害我定位了近一周，VLD内存泄漏工具也检测不出来
				cvReleaseCapture(&capture);
				//system("pause");
				return numFrames;
			}
		}*/
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, nPos);
		frame = cvRetrieveFrame(capture);
		//实际我的程序运行的过程中总是保存空的图像文件，这里加个保护
		if (!frame)
			break;
		//从视频中抓取的图像，保存的文件名:视频文件名称+_img_累加数字
		sprintf(chFilename, "%s_img_%d.jpg", fileName, count++);
		string tmpStr;
		//如果传入的参数saveImgPath为空，则将图片保存在当前相对路径下
		if (saveImgPath != NULL) {
			tmpStr = saveImgPath;
			int nLen = tmpStr.length();
			//根据传入的参数saveImgPath是否以“\”结尾而做不同的处理
			if (tmpStr[nLen - 1] != '\\') {
				strSaveImgPath = strSaveImgPath + saveImgPath + "\\" + chFilename;
			} 
			else {
				strSaveImgPath = strSaveImgPath + saveImgPath + chFilename;
			}
			//如果目录不存在，就逐层创建，自定义函数
			mkdirByLevel(saveImgPath);
		}
		else {
			strSaveImgPath = strSaveImgPath + chFilename;
		}
		
		cvSaveImage(strSaveImgPath.c_str(), frame);

		//把文件名称加入到vector中
		vImgPath.push_back(strSaveImgPath);

		nPos += period;

		//释放string内存
		strSaveImgPath.clear();
		string(strSaveImgPath).swap(strSaveImgPath);
		strSaveImgPath = "";
	}
	//释放对视频的引用，这里很关键，防止内存泄漏
	cvReleaseCapture(&capture);
	return numFrames;
}
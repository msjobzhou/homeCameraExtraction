#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <cstdio>  
#include <cstdlib>  
using namespace std;
int main_face_detect()
{

	// 【1】加载分类器  
	// 加载Haar特征检测分类器  
	// 将haarcascade_frontalface_alt2.xml拷至项目目录下，便于操作  
	const char *pstrCascadeFileName = "haarcascade_frontalface_alt2.xml";
	CvHaarClassifierCascade *pHaarCascade = NULL;

	pHaarCascade = (CvHaarClassifierCascade*)cvLoad(pstrCascadeFileName);
	if (!pHaarCascade) { printf("分类器加载失败\n"); return -1; }


	// 【2】载入图像  
	const char *pstrImageName = "D:\\test\\test_2.jpg";
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY); // 转化成灰度图，提高检测速度  

													// 标记颜色       
	CvScalar FaceCirclecolors[] =
	{
		{ { 0, 0, 255 } },
		{ { 0, 128, 255 } },
		{ { 0, 255, 255 } },
		{ { 0, 255, 0 } },
		{ { 255, 128, 0 } },
		{ { 255, 255, 0 } },
		{ { 255, 0, 0 } },
		{ { 255, 0, 255 } }
	};

	// 设置缓存区  
	CvMemStorage *pcvMStorage = cvCreateMemStorage(0);
	cvClearMemStorage(pcvMStorage);
	// 【3】识别  
	CvSeq *pcvSeqFaces = cvHaarDetectObjects(pGrayImage, pHaarCascade, pcvMStorage);
	printf("人脸个数: %d\n", pcvSeqFaces->total);

	// 【4】标记  
	for (int i = 0; i <pcvSeqFaces->total; i++)
	{
		CvRect* r = (CvRect*)cvGetSeqElem(pcvSeqFaces, i);
		CvPoint center;
		int radius;
		center.x = cvRound((r->x + r->width * 0.5));
		center.y = cvRound((r->y + r->height * 0.5));
		radius = cvRound((r->width + r->height) * 0.25);
		cvCircle(pSrcImage, center, radius, FaceCirclecolors[i % 8], 2);
	}
	cvReleaseMemStorage(&pcvMStorage); // 释放缓存  

									   // 【5】显示  
	const char *pstrWindowsTitle = "【人脸识别】";
	cvShowImage(pstrWindowsTitle, pSrcImage);

	cvWaitKey(0);
	return 0;
}
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <cstdio>  
#include <cstdlib>  
using namespace std;
int main_face_detect()
{

	// ��1�����ط�����  
	// ����Haar������������  
	// ��haarcascade_frontalface_alt2.xml������ĿĿ¼�£����ڲ���  
	const char *pstrCascadeFileName = "haarcascade_frontalface_alt2.xml";
	CvHaarClassifierCascade *pHaarCascade = NULL;

	pHaarCascade = (CvHaarClassifierCascade*)cvLoad(pstrCascadeFileName);
	if (!pHaarCascade) { printf("����������ʧ��\n"); return -1; }


	// ��2������ͼ��  
	const char *pstrImageName = "D:\\test\\test_2.jpg";
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY); // ת���ɻҶ�ͼ����߼���ٶ�  

													// �����ɫ       
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

	// ���û�����  
	CvMemStorage *pcvMStorage = cvCreateMemStorage(0);
	cvClearMemStorage(pcvMStorage);
	// ��3��ʶ��  
	CvSeq *pcvSeqFaces = cvHaarDetectObjects(pGrayImage, pHaarCascade, pcvMStorage);
	printf("��������: %d\n", pcvSeqFaces->total);

	// ��4�����  
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
	cvReleaseMemStorage(&pcvMStorage); // �ͷŻ���  

									   // ��5����ʾ  
	const char *pstrWindowsTitle = "������ʶ��";
	cvShowImage(pstrWindowsTitle, pSrcImage);

	cvWaitKey(0);
	return 0;
}
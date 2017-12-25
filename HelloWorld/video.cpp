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
�������ã�readvideo ������Ƶ��֡����-1ʱ��ʾ��Ƶ�ļ���Ч, 0��ʾ��֡���������ұ����ļ���ָ����·��
���������fileName ��Ҫ��ȡ��Ƶ�ļ���
		filePath ��Ҫ��ȡ��Ƶ�ļ�·�����������·��������"\"��β�����������л����ļ���ǰ��һ��"\"
		period ֡�����ÿ������֡ȡ����һ�Ž�ͼ
		saveImgPath ����Ƶ��ץȡ��ͼ���ŵ�·��
		vImgPath �Ѵ���Ƶ�ж�ȡ��ͼ��ȫ��������vector��������������
*/
int readVideoSaveImg(const char* fileName, const char* filePath, const char* saveImgPath, int period, vector<string> &vImgPath) {
	//int period;
	int count = 1; //�ļ���ſ�ʼ����ֵ
	
	string fileFullPath = "";
	fileFullPath = fileFullPath + filePath + "\\" + fileName;
	CvCapture *capture = cvCreateFileCapture(fileFullPath.c_str()); //����Ƶ�ļ�
	//��ȡ��Ƶ�ļ��ܹ��ж���֡
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	//��Ƶ�ļ���Ϊ�գ�ֱ�ӷ���-1
	if (capture == NULL)
	{
		return  -1;
	}
	//printf("totalFrameNum:%d",numFrames);
	IplImage *frame;
	string strSaveImgPath="";
	int nPos = period;
	//����Ƶ��ץȡ��ͼ�񣬱�����ļ���
	char chFilename[128];
	//while (1)
	while(nPos<= numFrames && cvGrabFrame(capture))
	{
		//��֡��ȡ�ķ�ʽ�ĳɻ�ȡ�ض�֡�ķ�ʽ������Ч��
		/*for (int i = 0; i < period; i++)
		{
			frame = cvQueryFrame(capture);
			//printf("query frame");
			if (!frame)
			{
				//printf("finish!\n");
				//����û���ͷ���Ƶ��Դ(����ĩβ�ͷ���Դ��cvReleaseCaptureʵ�����߲���)�����Ҷ�λ�˽�һ�ܣ�VLD�ڴ�й©����Ҳ��ⲻ����
				cvReleaseCapture(&capture);
				//system("pause");
				return numFrames;
			}
		}*/
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, nPos);
		frame = cvRetrieveFrame(capture);
		//ʵ���ҵĳ������еĹ��������Ǳ���յ�ͼ���ļ�������Ӹ�����
		if (!frame)
			break;
		//����Ƶ��ץȡ��ͼ�񣬱�����ļ���:��Ƶ�ļ�����+_img_�ۼ�����
		sprintf(chFilename, "%s_img_%d.jpg", fileName, count++);
		string tmpStr;
		//�������Ĳ���saveImgPathΪ�գ���ͼƬ�����ڵ�ǰ���·����
		if (saveImgPath != NULL) {
			tmpStr = saveImgPath;
			int nLen = tmpStr.length();
			//���ݴ���Ĳ���saveImgPath�Ƿ��ԡ�\����β������ͬ�Ĵ���
			if (tmpStr[nLen - 1] != '\\') {
				strSaveImgPath = strSaveImgPath + saveImgPath + "\\" + chFilename;
			} 
			else {
				strSaveImgPath = strSaveImgPath + saveImgPath + chFilename;
			}
			//���Ŀ¼�����ڣ�����㴴�����Զ��庯��
			mkdirByLevel(saveImgPath);
		}
		else {
			strSaveImgPath = strSaveImgPath + chFilename;
		}
		
		cvSaveImage(strSaveImgPath.c_str(), frame);

		//���ļ����Ƽ��뵽vector��
		vImgPath.push_back(strSaveImgPath);

		nPos += period;

		//�ͷ�string�ڴ�
		strSaveImgPath.clear();
		string(strSaveImgPath).swap(strSaveImgPath);
		strSaveImgPath = "";
	}
	//�ͷŶ���Ƶ�����ã�����ܹؼ�����ֹ�ڴ�й©
	cvReleaseCapture(&capture);
	return numFrames;
}
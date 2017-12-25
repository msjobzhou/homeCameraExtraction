// HelloWorld.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "video.h"

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"  
#include "opencv/cv.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include "traverseFolder.h"


#include<stdio.h>
#include<stdlib.h>

//#include <vld.h>

using namespace std;
using namespace cv;



/*
* imagefile1:
* imagefile2:
* method: could be CV_COMP_CHISQR, CV_COMP_BHATTACHARYYA, CV_COMP_CORREL, CV_COMP_INTERSECT

*/
float CompareHist(const char* imagefile1, const char* imagefile2, int nCompMethod)
{
	//��ֱ��ͼ��  
	int HistogramBins = 256;
	float HistogramRange1[2] = { 0,255 };
	float *HistogramRange[1] = { &HistogramRange1[0] };

	IplImage *image1 = cvLoadImage(imagefile1, 0);
	IplImage *image2 = cvLoadImage(imagefile2, 0);

	CvHistogram *Histogram1 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
	CvHistogram *Histogram2 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);

	cvCalcHist(&image1, Histogram1);
	cvCalcHist(&image2, Histogram2);

	cvNormalizeHist(Histogram1, 1);
	cvNormalizeHist(Histogram2, 1);
	float fSimilarity;
	char chResult[128];
	string strResult = "";
	switch(nCompMethod) {
		// CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA�����ֶ�����������ֱ��ͼ�ıȽϣ�ֵԽС��˵��ͼ��Խ����  
	case 1:
		fSimilarity = 1 - cvCompareHist(Histogram1, Histogram2, CV_COMP_CHISQR);
		sprintf(chResult, "CV_COMP_CHISQR : %.4f\n", fSimilarity);
		break;
	case 2:
		fSimilarity = 1 - cvCompareHist(Histogram1, Histogram2, CV_COMP_BHATTACHARYYA);
		sprintf(chResult, "CV_COMP_BHATTACHARYYA : %.4f\n", fSimilarity);
		break;
		
		// CV_COMP_CORREL, CV_COMP_INTERSECT������ֱ��ͼ�ıȽϣ�ֵԽ��˵��ͼ��Խ����  
	case 3:
		fSimilarity = cvCompareHist(Histogram1, Histogram2, CV_COMP_CORREL);
		sprintf(chResult, "CV_COMP_CORREL : %.4f\n", fSimilarity);
		break;
	case 4:
		fSimilarity = cvCompareHist(Histogram1, Histogram2, CV_COMP_INTERSECT);
		sprintf(chResult, "CV_COMP_INTERSECT : %.4f\n", fSimilarity);
		break;
	default: 
		fSimilarity = -0.01;
		chResult[0] = NULL;
		break;


	}
	strResult += chResult;
	
	cvReleaseImage(&image1);
	cvReleaseImage(&image2);
	cvReleaseHist(&Histogram1);
	cvReleaseHist(&Histogram2);
	return fSimilarity;
}
/*
	�����ļ��еĻص�����
	��ȡ��Ƶ�ļ�������һ��֡�������ͼ��֡�����imgͼ���ض�·��
*/
void tf_function(string &filePath, ofstream &outFile) {
	//���ݾ���·���ļ����õ��ļ���·�����ļ���
	char folder[255], fileName[255];

	getFolderAndFilename((char*)filePath.c_str(), folder, fileName);
	//�ڸ�Ŀ¼֮������tmp_comp_img��Ŀ¼����������Ŀ¼���ֺ���Ŀ¼�ṹһ��
	char * chPath = folder;
	//img�ļ��洢Ŀ¼
	string strImgPath = "";
	for (char* chTmp = chPath; *chTmp; chTmp++) {
		//�ҵ����̸�Ŀ¼
		strImgPath += *chTmp;
		if (*chTmp == ':'&&*(chTmp + 1) == '\\') {
			strImgPath += *(chTmp + 1);
			//�ڸ�Ŀ¼֮������tmp_comp_img��Ŀ¼
			strImgPath += "tmp_comp_img\\";
			char chBufTmp[256];
			strcpy(chBufTmp, chTmp + 2);
			strImgPath += chBufTmp;
			break;
		}
	}
	//printf("strImgPath:%s",strImgPath.c_str());
	vector<string> vImgPath;
	int nFrameNum;
	nFrameNum=readVideoSaveImg(fileName, chPath, strImgPath.c_str(), 50, vImgPath);

	if (vImgPath.size() < 2) {
		//��ȡ��ͼƬ����С�ڵ���2����ʱ��ֱ���˳�
		return;
	}


	//�Ƚ���Ƶץȡ����img�ļ������ҵó����ƶ���͵��ٽ�����ͼƬ����Ϊ������Ƶ�����Ե�˵�������Զ��ŷָ�����ʽд�뵽�ļ�outFile��
	float fSimilarity = 1.0f;
	int nIndex=1;
	//outFile << "vImgPath size:" << vImgPath.size()<<" ";
	for (int i = 1; i < vImgPath.size(); i++) {
		float fTmp;
		fTmp =CompareHist(vImgPath.at(i - 1).c_str(), vImgPath.at(i).c_str(),1);
		if (fTmp < fSimilarity)
		{
			fSimilarity = fTmp;
			nIndex = i;
		}
	}
	//outFile << "nIndex:" << nIndex << " ";
	//�����д�뵽�ļ��� ��ʽΪ�� ��Ƶ�ļ�����·��(filePath),֡��,���ƶ�(fSimilarity),ͼ���ļ�1(vImgPath[nIndex]),ͼ���ļ�2(vImgPath[nIndex-1])
	outFile << filePath << "," << nFrameNum << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
	cout << filePath << "," << nFrameNum << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
	
	fSimilarity = 1.0f;
	nIndex = 1;
	//outFile << "vImgPath size:" << vImgPath.size() << " ";
	
	for (int i = 1; i < vImgPath.size(); i++) {
		float fTmp;
		fTmp = CompareHist(vImgPath.at(i - 1).c_str(), vImgPath.at(i).c_str(), 2);
		if (fTmp < fSimilarity)
		{
			fSimilarity = fTmp;
			nIndex = i;
		}
	}
	//outFile << "nIndex:" << nIndex << " ";
	//�����д�뵽�ļ��� ��ʽΪ�� ��Ƶ�ļ�����·��(filePath),���ƶ�(fSimilarity),ͼ���ļ�1(vImgPath[nIndex]),ͼ���ļ�2(vImgPath[nIndex-1])
	outFile << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
	cout << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);

	fSimilarity = 1.0f;
	nIndex = 1;
	//outFile << "vImgPath size:" << vImgPath.size() << " ";
	for (int i = 1; i < vImgPath.size(); i++) {
		float fTmp;
		fTmp = CompareHist(vImgPath.at(i - 1).c_str(), vImgPath.at(i).c_str(), 3);
		if (fTmp < fSimilarity)
		{
			fSimilarity = fTmp;
			nIndex = i;
		}




	}
	//outFile << "nIndex:" << nIndex << " ";
	//�����д�뵽�ļ��� ��ʽΪ�� ��Ƶ�ļ�����·��(filePath),���ƶ�(fSimilarity),ͼ���ļ�1(vImgPath[nIndex]),ͼ���ļ�2(vImgPath[nIndex-1])
	outFile << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
	cout << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);

	fSimilarity = 1.0f;
	nIndex = 1;
	//outFile << "vImgPath size:" << vImgPath.size() << " ";
	for (int i = 1; i < vImgPath.size(); i++) {
		float fTmp;
		fTmp = CompareHist(vImgPath.at(i - 1).c_str(), vImgPath.at(i).c_str(), 4);
		if (fTmp < fSimilarity)
		{
			fSimilarity = fTmp;
			nIndex = i;
		}
	}
	//outFile << "nIndex:" << nIndex << " ";
	//�����д�뵽�ļ��� ��ʽΪ�� ��Ƶ�ļ�����·��(filePath),���ƶ�(fSimilarity),ͼ���ļ�1(vImgPath[nIndex]),ͼ���ļ�2(vImgPath[nIndex-1])
	outFile << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex) <<endl;
	cout << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex) <<endl;
	
	//�ͷ�vector������ڴ�ռ䣬��ֹ�ڴ�й©
	vImgPath.clear();
	vector<string>(vImgPath).swap(vImgPath);
}

int main(int argc, char* argv[])
{
	
	
	traverseFolder_handler tf_handler = tf_function;

	
	//CompareHist("img_39.jpg", "img_40.jpg");
	//�ֱ������������ͷ�����һ����������
	//ofstream of("E:\\test\\����ǽ��.txt"/*, ios::app*/);
	//string path = "E:\\��������Ƶ����\\����ǽ��\\2017-11-06";
	

	ofstream of("E:\\test\\�������ӹ���.txt"/*, ios::app*/);
	string path = "E:\\��������Ƶ����\\�������ӹ���\\2017-11-06";
	
	traverseFolder(path, of, tf_handler);
	of.close();

	//int *p_int = new int(12345);

	//cout << *p_int << endl;
	//free(p_int);

	//�˺����ȴ�������������������ͷ���
	waitKey();

	return 0;
}

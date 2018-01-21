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

#include <assert.h>
#include<stdio.h>
#include<stdlib.h>

#include <cv.h>

//#include <vld.h>

using namespace std;
using namespace cv;

static const unsigned int guBufferSize = 100;
static const string gsLRCF = "\n";

struct experimentalValue {
	float fSimilarityThreshold; //������Ƶ��ץȡ��ͼƬ���ƶ���ֵ�����������ֵ��ɾ����С�ڵı�����Ӳ����
	//����ͷ�������Ƶ����1����Ϊ��λ�洢�ģ�1���ӵ�������Ƶ���ܴ��ڡ�����������һ��������һ���ط�һֱ���飬
	//������仯��������Ρ���ԭ����Щ��Ƶ�����ƶ��ж���Ҫɾ����,Ҳ����˵�ǡ���ɾ��
	//���������Ż��㷨���������֡���ɾ���ĸ���
	//������˵����ǰ��ƵA�����ƶ���ֵС��fSimilarityThreshold�����Ϊ��ɾ��, nConsecutiveVideoNum���ó�6
    //��A֮ǰ�Ľ�����A�ĵ�һ����Ƶ��ΪB1���ڶ�����Ƶ��ΪB2....�Դ����Ƶ�B5  ��Ƶ�����������ģ�B5��...,B1,A
	//1�����ȶ��壺������仯�Ƚϴ󡱵���Ƶ��ָͨ���ж� ����Ƶ�����ƶȺ�fSimilarityThreshold�Ĵ�С��С����ֵ��
	//2�������ǰ������Ƶ������仯�Ƚϴ��ж���Ҫ����Ļ�������������
	//3�����ȴ�B5��ʼ�ж����,B5ͨ��fSimilarityThreshold�Ƚ��жϡ�����仯�Ƚϴ���Ҫ����Ļ�����B5��A��6����Ƶ�����Ϊ��Ҫ����
	//4�����B5ͨ��fSimilarityThreshold�жϺ���Ҫ����Ļ������ж�B4��ͬ��������������3�������Ļ�,B4��A5����Ƶ���棬�Դ�����
	int nConsecutiveVideoNum;
};
typedef struct experimentalValue experimentalValue;
//����ǽ������ͷ����ֵ
experimentalValue gCamOnLivingRoomWallExperValue = {0.96, 6};





struct write2FileBuf {
	//��Ƶ�ļ��Ĵ��������ַ�������ʽ���ڴ������У������ʽ��tf_function2�ж�Ӧ��˵��
	string videoRecord[guBufferSize];
	//���ݴ���Ƶ�ļ���ȡ��ͼ��֡���õ������ƶ� ��ֱ��ͼ�Ƚϣ�
	float videoSimilarity[guBufferSize];
	//ͼ����Ƶ�Ĵ������� ��ǰ0��ʾ������1ɾ������ʼĬ��Ϊ����
	int videoResult[guBufferSize];
	int nCurPos = 0;
	string videoFileFullPath[guBufferSize];
};

typedef struct write2FileBuf write2FileBuf;

write2FileBuf gVideoSimilarityBuf;

/*
* image1:
* image2:
* method: could be CV_COMP_CHISQR, CV_COMP_BHATTACHARYYA, CV_COMP_CORREL, CV_COMP_INTERSECT

*/
float CompareHist(IplImage *image1, IplImage *image2, int nCompMethod) {
	//��ֱ��ͼ��  
	int HistogramBins = 256;
	float HistogramRange1[2] = { 0, 255 };
	float *HistogramRange[1] = { &HistogramRange1[0] };
	//cout <<"before cvCreateHist" << endl;
	CvHistogram *Histogram1 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
	CvHistogram *Histogram2 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
	//cout << "before cvCalcHist" << endl;
	cvCalcHist(&image1, Histogram1);
	cvCalcHist(&image2, Histogram2);
	//cout << "before cvNormalizeHist" << endl;
	cvNormalizeHist(Histogram1, 1);
	cvNormalizeHist(Histogram2, 1);
	float fSimilarity;
	char chResult[128];
	string strResult = "";
	switch (nCompMethod) {
		// CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA�����ֶ�����������ֱ��ͼ�ıȽϣ�ֵԽС��˵��ͼ��Խ����  
	case 1:
		//cout << "switch case1 cvCompareHist" << endl;
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
	//cout << "before cvReleaseImage" << endl;
	//�˺����ⲿ�����߾���image���ͷ�ʱ��
	//cvReleaseImage(&image1);
	//cvReleaseImage(&image2);
	//cout << "before cvReleaseHist" << endl;
	cvReleaseHist(&Histogram1);
	cvReleaseHist(&Histogram2);
	//cout << "before return" << endl;
	return fSimilarity;

}
float CompareHist(const char* imagefile1, const char* imagefile2, int nCompMethod)
{
	
	IplImage *image1 = cvLoadImage(imagefile1, 0);
	IplImage *image2 = cvLoadImage(imagefile2, 0);

	float fRet = CompareHist(image1, image2, nCompMethod);

	cvReleaseImage(&image1);
	cvReleaseImage(&image2);

	return fRet;
}
/*
	�����ļ��еĻص�����
	��ȡ��Ƶ�ļ�������һ��֡�������ͼ��֡�����imgͼ���ض�·��,֮������ֱ��ͼ�㷨�Ƚ�ͼƬ�����ԣ��������д���ض����ļ�
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
	nFrameNum=readVideoSaveImg(fileName, chPath, strImgPath.c_str(), 100, vImgPath);

	if (vImgPath.size() < 2) {
		//��ȡ��ͼƬ����С�ڵ���2����ʱ��ֱ���˳�
		return;
	}


	//�Ƚ���Ƶץȡ����img�ļ������ҵó����ƶ���͵��ٽ�����ͼƬ����Ϊ������Ƶ�����Ե�˵�������Զ��ŷָ�����ʽд�뵽�ļ�outFile��
	//4��ֱ��ͼ�ȽϷ�ʽ������һ��
	for (int nCompareMethod = 1; nCompareMethod <= 4; nCompareMethod++) {
		float fSimilarity = 1.0f;
		int nIndex=1;
		//outFile << "vImgPath size:" << vImgPath.size()<<" ";
		for (int i = 1; i < vImgPath.size(); i++) {
			float fTmp;
			fTmp = CompareHist(vImgPath.at(i - 1).c_str(), vImgPath.at(i).c_str(), nCompareMethod);
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
	}
	outFile <<endl;
	cout <<endl;
	
	//�ͷ�vector������ڴ�ռ䣬��ֹ�ڴ�й©
	vImgPath.clear();
	vector<string>(vImgPath).swap(vImgPath);
}


/*
�����ļ��еĻص�����2
��ȡ��Ƶ�ļ�������һ��֡�������ͼ��֡��ȡ���ڴ棬֮������ֱ��ͼ�㷨�Ƚ�ͼƬ�����ԣ��������д���ض����ļ�
*/
void tf_function2(string &filePath, ofstream &outFile) {
	//���ݾ���·���ļ����õ��ļ���·�����ļ���
	char folder[255], fileName[255];

	getFolderAndFilename((char*)filePath.c_str(), folder, fileName);
	//�ڸ�Ŀ¼֮������tmp_comp_img��Ŀ¼����������Ŀ¼���ֺ���Ŀ¼�ṹһ��
	char * chPath = folder;
	
	//printf("strImgPath:%s",strImgPath.c_str());
	vector<IplImage*> vImg;
	int nFrameNum;
	nFrameNum = readVideo(fileName, chPath, 100, vImg);
	//cout << "after readVideo called" << endl;
	if (vImg.size()< 2) {
		//��ȡ��ͼƬ����С�ڵ���2����ʱ��ֱ���˳�
		if (1 == vImg.size())
			cvReleaseImage(&(vImg.at(0)));
		return;
	}
	//cvNamedWindow("Gray", CV_WINDOW_AUTOSIZE);
	//cvShowImage("Gray", vImg.at(0));
	//�Ƚ���Ƶץȡ����img�ļ������ҵó����ƶ���͵��ٽ�����ͼƬ����Ϊ������Ƶ�����Ե�˵�������Զ��ŷָ�����ʽд�뵽�ļ�outFile��
	//4��ֱ��ͼ�ȽϷ�ʽ������һ�£�ȡfSimilarity��С��һ�ֱȽϷ���
	float fSimilarityMin = 1.0f;
	for (int nCompareMethod = 1; nCompareMethod <= 4; nCompareMethod++) {
		float fSimilarity = 1.0f;
		int nIndex = 1;
		//outFile << "vImgPath size:" << vImgPath.size()<<" ";
		for (int i = 1; i < vImg.size(); i++) {
			float fTmp;
			fTmp = CompareHist(vImg.at(i - 1), vImg.at(i), nCompareMethod);
			//ʵ�ʱȽϹ����з���fTmp��ż�����ִ���1����С��0.3���������ʱ��fTmp����Ϊ1������fTmp������Сֵ�Ƚ�ʱ��
			//����ȥ�����൱���޳�����Ч����
			if (fTmp>1 || fTmp <= 0.3) fTmp = 1;
			if (fTmp < fSimilarity)
			{
				fSimilarity = fTmp;
				nIndex = i;
			}
		}
		if (fSimilarity < fSimilarityMin) fSimilarityMin = fSimilarity;
	}

	//�����д�뵽�ļ��� ��ʽΪ�� ��Ƶ�ļ�����·��(filePath),֡��,���ƶ�(fSimilarity)
	//outFile << filePath << "," << nFrameNum << "," << fSimilarityMin << endl;
	//cout << filePath << "," << nFrameNum << "," << fSimilarityMin << endl;
	//�����д�뻺��gVideoSimilarityBuf ��������֮��һ����д��Ӳ�̣���������Ч��
	char chResult[256];
	//chResult���������ʽnFrameNum +","+ fSimilarityMin 
	sprintf(chResult, "%d,%.4f", nFrameNum, fSimilarityMin);
	//cout <<"chResult: "<< chResult << endl;
	string str2Write;
	str2Write = str2Write + filePath + "," + chResult;
	//cout << "str2Write: " << str2Write << endl;
	assert(gVideoSimilarityBuf.nCurPos <= guBufferSize-1);
	gVideoSimilarityBuf.videoFileFullPath[gVideoSimilarityBuf.nCurPos] = filePath;
	//��Ƶ�ļ����������ַ�����ʽд�����飺�����ʽΪ��
	//�ļ�����·����filePath����Ƶ�ļ�֡��nFrameNum��ֱ��ͼ�Ƚϵ�����ͼƬ���ƶ�
	gVideoSimilarityBuf.videoRecord[gVideoSimilarityBuf.nCurPos] = str2Write;
	gVideoSimilarityBuf.videoSimilarity[gVideoSimilarityBuf.nCurPos] = fSimilarityMin;
	//����Ƶ����ȡ��ͼƬ������ƶȱȽϸߣ�fSimilarityMinԽ�ӽ�1Խ���ƣ�������Ƶ�ļ����ݱ仯����
	//��videoResult��Ϊ1������Ƶ�����ΪҪɾ����0.96�ǿ���ǽ������ͷ�ľ���ֵ����ÿ������ͷ���������ֵ��һ��
	
	if (gVideoSimilarityBuf.videoSimilarity[gVideoSimilarityBuf.nCurPos] > gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
		gVideoSimilarityBuf.videoResult[gVideoSimilarityBuf.nCurPos] = 1;
	}
	else {
		gVideoSimilarityBuf.videoResult[gVideoSimilarityBuf.nCurPos] = 0;
	}

	gVideoSimilarityBuf.nCurPos++;
	//��������������һ��д���ļ�
	//
	if (gVideoSimilarityBuf.nCurPos == guBufferSize) {
		const int nConsecutiveVideoNum = gCamOnLivingRoomWallExperValue.nConsecutiveVideoNum;
		string strTmp;
		//����ѭ��ɨ�軺���е���Ƶ����һ�齫������ɾ������Ƶ��ǳ���
		for (int nPos = 1; nPos <= guBufferSize; nPos++) {
			//����Ƶ����ɾ���㷨������ο�struct experimentalValue�е�˵��
			//��ǰ��Ƶ���Ϊ��ɾ��ʱ��ͨ���Ƚ���Ƶ�����ƶȻ�ã�
			if ((gVideoSimilarityBuf.videoSimilarity[nPos - 1] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold)
				&& (nPos >= nConsecutiveVideoNum)) {
				//�𽥼��ٱȽϲ���nStep
				int nStep = nConsecutiveVideoNum-1;
				while (nStep >=1 ) {
					//���뵱ǰ��Ƶ֮ǰ�ĵ�nStep����ƵҲ�ǲ�ɾ�������ʱ��ͨ���Ƚ���Ƶ�����ƶȻ�ã�
					if (gVideoSimilarityBuf.videoSimilarity[(nPos - 1) - nStep] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
						//����ǰ��Ƶ�͡����뵱ǰ��Ƶ֮ǰ�ĵ�nStep����Ƶ��֮�����Ƶȫ�����Ϊ��ɾ��
						for (int i = 1; i <= nStep; i++) {
							gVideoSimilarityBuf.videoResult[nPos - 1 - i] = 0;
						}
						break;
					}
					nStep--;
				}
			}
		}
		//�ڶ���ɨ�軺���е���Ƶ������Ƶ��������Ӧ��write2FileBuf�ṹ��д���ļ�
		for (int nPos = 0; nPos < guBufferSize; nPos++) {
			strTmp += gVideoSimilarityBuf.videoRecord[nPos];
			char chTmp[256];
			//chTmp���Ƿ�ɾ���ı��д��
			sprintf(chTmp, ",%d", gVideoSimilarityBuf.videoResult[nPos]);
			strTmp = strTmp+chTmp+gsLRCF;
			if (1 == gVideoSimilarityBuf.videoResult[nPos]) {
				if (remove(gVideoSimilarityBuf.videoFileFullPath[nPos].c_str()))
					cout << "ɾ���ļ�ʧ�ܣ�" << gVideoSimilarityBuf.videoFileFullPath[nPos] << endl;
			}
		}

		outFile << strTmp;
		//cout << "strTmp:" << strTmp;
		outFile.flush();
		cout << "һ����д��" << guBufferSize << "����Ƶ�ļ����ƶ���ȡ���" << endl;
		gVideoSimilarityBuf.nCurPos = 0;
	}
		
	//�ͷ�vector������ڴ�ռ䣬��ֹ�ڴ�й©
	for (int i = 0; i < vImg.size(); i++) {
		cvReleaseImage(&(vImg.at(i)));
	}

	vImg.clear();
	vector<IplImage*>(vImg).swap(vImg);
}

int main(int argc, char* argv[])
{
	traverseFolder_handler tf_handler = tf_function2;
	//traverseFolder_handler tf_handler = tf_function2;
	
	//�ֱ������������ͷ�����һ����������
	//ofstream of("E:\\test\\����ǽ��.txt"/*, ios::app*/);
	//string path = "E:\\��������Ƶ����\\����ǽ��\\2017-11-06";
	

	ofstream of("E:\\test\\��������Ƶ����.txt"/*, ios::app*/);
	string path = "E:\\��������Ƶ����";

	//ofstream of("D:\\test\\����ǽ��.txt"/*, ios::app*/);
	//string path = "D:\\��������Ƶ����2017��10��23�յ�11��12��\\����ǽ��";
    
	traverseFolder(path, of, tf_handler);
	//����һ��д��ʣ�µ��ļ�
	if (gVideoSimilarityBuf.nCurPos >= 1) {
		const int nConsecutiveVideoNum = gCamOnLivingRoomWallExperValue.nConsecutiveVideoNum;
		string strTmp;
		//����ѭ��ɨ�軺���е���Ƶ����һ�齫������ɾ������Ƶ��ǳ���
		for (int nPos = 1; nPos <= gVideoSimilarityBuf.nCurPos; nPos++) {
			//����Ƶ����ɾ���㷨������ο�struct experimentalValue�е�˵��
			//��ǰ��Ƶ���Ϊ��ɾ��ʱ��ͨ���Ƚ���Ƶ�����ƶȻ�ã�
			if ((gVideoSimilarityBuf.videoSimilarity[nPos - 1] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold)
				&& (nPos >= nConsecutiveVideoNum)) {
				//�𽥼��ٱȽϲ���nStep
				int nStep = nConsecutiveVideoNum - 1;
				while (nStep >= 1) {
					//���뵱ǰ��Ƶ֮ǰ�ĵ�nStep����ƵҲ�ǲ�ɾ�������ʱ��ͨ���Ƚ���Ƶ�����ƶȻ�ã�
					if (gVideoSimilarityBuf.videoSimilarity[(nPos - 1) - nStep] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
						//����ǰ��Ƶ�͡����뵱ǰ��Ƶ֮ǰ�ĵ�nStep����Ƶ��֮�����Ƶȫ�����Ϊ��ɾ��
						for (int i = 1; i <= nStep; i++) {
							gVideoSimilarityBuf.videoResult[nPos - 1 - i] = 0;
						}
						break;
					}
					nStep--;
				}
			}
		}
		//�ڶ���ɨ�軺���е���Ƶ������Ƶ��������Ӧ��write2FileBuf�ṹ��д���ļ�
		for (int nPos = 0; nPos < gVideoSimilarityBuf.nCurPos; nPos++) {
			strTmp += gVideoSimilarityBuf.videoRecord[nPos];
			char chTmp[256];
			//chTmp���Ƿ�ɾ���ı��д��
			sprintf(chTmp, ",%d", gVideoSimilarityBuf.videoResult[nPos]);
			strTmp = strTmp + chTmp + gsLRCF;
			if (1 == gVideoSimilarityBuf.videoResult[nPos]) {
				if (remove(gVideoSimilarityBuf.videoFileFullPath[nPos].c_str()))
					cout << "ɾ���ļ�ʧ�ܣ�" << gVideoSimilarityBuf.videoFileFullPath[nPos] << endl;
			}
		}

		of << strTmp;
		//cout << "strTmp:" << strTmp;
		of.flush();
		cout << "һ����д��" << gVideoSimilarityBuf.nCurPos << "����Ƶ�ļ����ƶ���ȡ���" << endl;
		gVideoSimilarityBuf.nCurPos = 0;
	}
	of.close();

	//�˺����ȴ�������������������ͷ���
	waitKey();
	
	return 0;
}

// HelloWorld.cpp : 定义控制台应用程序的入口点。
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
* image1:
* image2:
* method: could be CV_COMP_CHISQR, CV_COMP_BHATTACHARYYA, CV_COMP_CORREL, CV_COMP_INTERSECT

*/
float CompareHist(IplImage *image1, IplImage *image2, int nCompMethod) {
	//画直方图用  
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
		// CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA这两种都可以用来做直方图的比较，值越小，说明图形越相似  
	case 1:
		//cout << "switch case1 cvCompareHist" << endl;
		fSimilarity = 1 - cvCompareHist(Histogram1, Histogram2, CV_COMP_CHISQR);
		sprintf(chResult, "CV_COMP_CHISQR : %.4f\n", fSimilarity);
		break;
	case 2:
		fSimilarity = 1 - cvCompareHist(Histogram1, Histogram2, CV_COMP_BHATTACHARYYA);
		sprintf(chResult, "CV_COMP_BHATTACHARYYA : %.4f\n", fSimilarity);
		break;

		// CV_COMP_CORREL, CV_COMP_INTERSECT这两种直方图的比较，值越大，说明图形越相似  
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
	//此函数外部调用者决定image的释放时机
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
	遍历文件夹的回调函数
	读取视频文件并按照一定帧数间隔将图像帧保存成img图像到特定路径,之后利用直方图算法比较图片相似性，并将结果写入特定的文件
*/
void tf_function(string &filePath, ofstream &outFile) {
	//根据绝对路径文件名得到文件夹路径和文件名
	char folder[255], fileName[255];

	getFolderAndFilename((char*)filePath.c_str(), folder, fileName);
	//在根目录之后增加tmp_comp_img子目录，其他各级目录保持后续目录结构一致
	char * chPath = folder;
	//img文件存储目录
	string strImgPath = "";
	for (char* chTmp = chPath; *chTmp; chTmp++) {
		//找到磁盘根目录
		strImgPath += *chTmp;
		if (*chTmp == ':'&&*(chTmp + 1) == '\\') {
			strImgPath += *(chTmp + 1);
			//在根目录之后增加tmp_comp_img子目录
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
		//获取的图片个数小于等于2个的时候直接退出
		return;
	}


	//比较视频抓取到的img文件，并且得出相似度最低的临近两个图片，作为整个视频差异性的说明，并以逗号分隔的形式写入到文件outFile中
	//4种直方图比较方式都尝试一下
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
		//将结果写入到文件中 格式为： 视频文件完整路径(filePath),帧数,相似度(fSimilarity),图像文件1(vImgPath[nIndex]),图像文件2(vImgPath[nIndex-1])
		outFile << filePath << "," << nFrameNum << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
		cout << filePath << "," << nFrameNum << "," << fSimilarity << "," << vImgPath.at(nIndex - 1) << "," << vImgPath.at(nIndex);
	}
	outFile <<endl;
	cout <<endl;
	
	//释放vector申请的内存空间，防止内存泄漏
	vImgPath.clear();
	vector<string>(vImgPath).swap(vImgPath);
}


/*
遍历文件夹的回调函数2
读取视频文件并按照一定帧数间隔将图像帧获取到内存，之后利用直方图算法比较图片相似性，并将结果写入特定的文件
*/
void tf_function2(string &filePath, ofstream &outFile) {
	//根据绝对路径文件名得到文件夹路径和文件名
	char folder[255], fileName[255];

	getFolderAndFilename((char*)filePath.c_str(), folder, fileName);
	//在根目录之后增加tmp_comp_img子目录，其他各级目录保持后续目录结构一致
	char * chPath = folder;
	
	//printf("strImgPath:%s",strImgPath.c_str());
	vector<IplImage*> vImg;
	int nFrameNum;
	nFrameNum = readVideo(fileName, chPath, 50, vImg);
	//cout << "after readVideo called" << endl;
	if (vImg.size() < 2) {
		//获取的图片个数小于等于2个的时候直接退出
		cvReleaseImage(&(vImg.at(0)));
		return;
	}
	//cvNamedWindow("Gray", CV_WINDOW_AUTOSIZE);
	//cvShowImage("Gray", vImg.at(0));
	//比较视频抓取到的img文件，并且得出相似度最低的临近两个图片，作为整个视频差异性的说明，并以逗号分隔的形式写入到文件outFile中
	//4种直方图比较方式都尝试一下
	for (int nCompareMethod = 1; nCompareMethod <= 4; nCompareMethod++) {
		float fSimilarity = 1.0f;
		int nIndex = 1;
		//outFile << "vImgPath size:" << vImgPath.size()<<" ";
		for (int i = 1; i < vImg.size(); i++) {
			float fTmp;
			fTmp = CompareHist(vImg.at(i - 1), vImg.at(i), nCompareMethod);
			if (fTmp < fSimilarity)
			{
				fSimilarity = fTmp;
				nIndex = i;
			}
		}
		//outFile << "nIndex:" << nIndex << " ";
		//将结果写入到文件中 格式为： 视频文件完整路径(filePath),帧数,相似度(fSimilarity),图像文件1(vImgPath[nIndex]),图像文件2(vImgPath[nIndex-1])
		outFile << filePath << "," << nFrameNum << "," << fSimilarity ;
		cout << filePath << "," << nFrameNum << "," << fSimilarity ;
	}
	outFile << endl;
	cout << endl;

	//释放vector申请的内存空间，防止内存泄漏
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
	
	//分别分析三类摄像头拍摄的一天数据样本
	//ofstream of("E:\\test\\餐厅墙上.txt"/*, ios::app*/);
	//string path = "E:\\周晓董视频备份\\餐厅墙上\\2017-11-06";
	

	//ofstream of("E:\\test\\客厅电视柜上.txt"/*, ios::app*/);
	//string path = "E:\\周晓董视频备份\\客厅电视柜上\\2017-11-06";

	ofstream of("E:\\test\\客厅墙上.txt"/*, ios::app*/);
	string path = "E:\\周晓董视频备份\\客厅墙上\\2017-08-13";
	
	traverseFolder(path, of, tf_handler);
	of.close();

	

	//此函数等待按键，按键盘任意键就返回
	waitKey();

	return 0;
}

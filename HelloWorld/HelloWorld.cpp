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
	float fSimilarityThreshold; //单个视频中抓取的图片相似度阈值，大于这个阈值的删除，小于的保留在硬盘上
	//摄像头拍摄的视频是以1分钟为单位存储的，1分钟的整个视频可能存在“诸如周晓董一个人坐在一个地方一直读书，
	//而画面变化不大的情形”，原本这些视频按相似度判断是要删除的,也可以说是“误删”
	//采用如下优化算法，减少这种“误删”的概率
	//举例来说，当前视频A，相似度阈值小于fSimilarityThreshold，标记为不删除, nConsecutiveVideoNum设置成6
    //在A之前的紧挨着A的第一个视频称为B1，第二个视频称为B2....以此类推到B5  视频序列是这样的：B5，...,B1,A
	//1、首先定义：“画面变化比较大”的视频是指通过判断 此视频的相似度和fSimilarityThreshold的大小，小于阈值的
	//2、如果当前单个视频“画面变化比较大”判断需要保存的话，继续往下走
	//3、首先从B5开始判断如果,B5通过fSimilarityThreshold比较判断“画面变化比较大”需要保存的话，则B5到A的6个视频都标记为需要保存
	//4、如果B5通过fSimilarityThreshold判断后不需要保存的话，再判断B4，同样满足上述步骤3中条件的话,B4到A5个视频保存，以此类推
	int nConsecutiveVideoNum;
};
typedef struct experimentalValue experimentalValue;
//客厅墙上摄像头经验值
experimentalValue gCamOnLivingRoomWallExperValue = {0.96, 6};





struct write2FileBuf {
	//视频文件的处理结果以字符串的形式放在此数组中，具体格式见tf_function2中对应的说明
	string videoRecord[guBufferSize];
	//根据从视频文件抽取的图像帧而得到的相似度 （直方图比较）
	float videoSimilarity[guBufferSize];
	//图像视频的处理结果： 当前0表示保留、1删除；初始默认为保留
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
	nFrameNum=readVideoSaveImg(fileName, chPath, strImgPath.c_str(), 100, vImgPath);

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
	nFrameNum = readVideo(fileName, chPath, 100, vImg);
	//cout << "after readVideo called" << endl;
	if (vImg.size()< 2) {
		//获取的图片个数小于等于2个的时候直接退出
		if (1 == vImg.size())
			cvReleaseImage(&(vImg.at(0)));
		return;
	}
	//cvNamedWindow("Gray", CV_WINDOW_AUTOSIZE);
	//cvShowImage("Gray", vImg.at(0));
	//比较视频抓取到的img文件，并且得出相似度最低的临近两个图片，作为整个视频差异性的说明，并以逗号分隔的形式写入到文件outFile中
	//4种直方图比较方式都尝试一下，取fSimilarity最小的一种比较方法
	float fSimilarityMin = 1.0f;
	for (int nCompareMethod = 1; nCompareMethod <= 4; nCompareMethod++) {
		float fSimilarity = 1.0f;
		int nIndex = 1;
		//outFile << "vImgPath size:" << vImgPath.size()<<" ";
		for (int i = 1; i < vImg.size(); i++) {
			float fTmp;
			fTmp = CompareHist(vImg.at(i - 1), vImg.at(i), nCompareMethod);
			//实际比较过程中发现fTmp会偶尔出现大于1或者小于0.3的情况，此时将fTmp设置为1，这样fTmp参与最小值比较时，
			//将被去除，相当于剔除此无效数据
			if (fTmp>1 || fTmp <= 0.3) fTmp = 1;
			if (fTmp < fSimilarity)
			{
				fSimilarity = fTmp;
				nIndex = i;
			}
		}
		if (fSimilarity < fSimilarityMin) fSimilarityMin = fSimilarity;
	}

	//将结果写入到文件中 格式为： 视频文件完整路径(filePath),帧数,相似度(fSimilarity)
	//outFile << filePath << "," << nFrameNum << "," << fSimilarityMin << endl;
	//cout << filePath << "," << nFrameNum << "," << fSimilarityMin << endl;
	//结果先写入缓存gVideoSimilarityBuf 缓存满了之后一次性写入硬盘，提升运行效率
	char chResult[256];
	//chResult变量输出格式nFrameNum +","+ fSimilarityMin 
	sprintf(chResult, "%d,%.4f", nFrameNum, fSimilarityMin);
	//cout <<"chResult: "<< chResult << endl;
	string str2Write;
	str2Write = str2Write + filePath + "," + chResult;
	//cout << "str2Write: " << str2Write << endl;
	assert(gVideoSimilarityBuf.nCurPos <= guBufferSize-1);
	gVideoSimilarityBuf.videoFileFullPath[gVideoSimilarityBuf.nCurPos] = filePath;
	//视频文件处理结果以字符串形式写入数组：具体格式为，
	//文件完整路径名filePath、视频文件帧数nFrameNum、直方图比较的相邻图片相似度
	gVideoSimilarityBuf.videoRecord[gVideoSimilarityBuf.nCurPos] = str2Write;
	gVideoSimilarityBuf.videoSimilarity[gVideoSimilarityBuf.nCurPos] = fSimilarityMin;
	//从视频中提取的图片如果相似度比较高（fSimilarityMin越接近1越相似），单视频文件内容变化不大
	//则videoResult置为1，该视频被标记为要删除，0.96是客厅墙上摄像头的经验值，且每个摄像头的这个经验值不一样
	
	if (gVideoSimilarityBuf.videoSimilarity[gVideoSimilarityBuf.nCurPos] > gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
		gVideoSimilarityBuf.videoResult[gVideoSimilarityBuf.nCurPos] = 1;
	}
	else {
		gVideoSimilarityBuf.videoResult[gVideoSimilarityBuf.nCurPos] = 0;
	}

	gVideoSimilarityBuf.nCurPos++;
	//缓存已满，批量一次写入文件
	//
	if (gVideoSimilarityBuf.nCurPos == guBufferSize) {
		const int nConsecutiveVideoNum = gCamOnLivingRoomWallExperValue.nConsecutiveVideoNum;
		string strTmp;
		//两遍循环扫描缓存中的视频，第一遍将防“误删”的视频标记出来
		for (int nPos = 1; nPos <= guBufferSize; nPos++) {
			//防视频“误删”算法，具体参考struct experimentalValue中的说明
			//当前视频标记为不删除时（通过比较视频的相似度获得）
			if ((gVideoSimilarityBuf.videoSimilarity[nPos - 1] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold)
				&& (nPos >= nConsecutiveVideoNum)) {
				//逐渐减少比较步长nStep
				int nStep = nConsecutiveVideoNum-1;
				while (nStep >=1 ) {
					//距离当前视频之前的第nStep个视频也是不删除的情况时（通过比较视频的相似度获得）
					if (gVideoSimilarityBuf.videoSimilarity[(nPos - 1) - nStep] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
						//将当前视频和“距离当前视频之前的第nStep个视频”之间的视频全部标记为不删除
						for (int i = 1; i <= nStep; i++) {
							gVideoSimilarityBuf.videoResult[nPos - 1 - i] = 0;
						}
						break;
					}
					nStep--;
				}
			}
		}
		//第二遍扫描缓存中的视频，将视频处理结果对应的write2FileBuf结构体写入文件
		for (int nPos = 0; nPos < guBufferSize; nPos++) {
			strTmp += gVideoSimilarityBuf.videoRecord[nPos];
			char chTmp[256];
			//chTmp将是否删除的标记写入
			sprintf(chTmp, ",%d", gVideoSimilarityBuf.videoResult[nPos]);
			strTmp = strTmp+chTmp+gsLRCF;
			if (1 == gVideoSimilarityBuf.videoResult[nPos]) {
				if (remove(gVideoSimilarityBuf.videoFileFullPath[nPos].c_str()))
					cout << "删除文件失败：" << gVideoSimilarityBuf.videoFileFullPath[nPos] << endl;
			}
		}

		outFile << strTmp;
		//cout << "strTmp:" << strTmp;
		outFile.flush();
		cout << "一次性写入" << guBufferSize << "个视频文件相似度提取结果" << endl;
		gVideoSimilarityBuf.nCurPos = 0;
	}
		
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
	

	ofstream of("E:\\test\\周晓董视频备份.txt"/*, ios::app*/);
	string path = "E:\\周晓董视频备份";

	//ofstream of("D:\\test\\客厅墙上.txt"/*, ios::app*/);
	//string path = "D:\\周晓董视频备份2017年10月23日到11月12日\\客厅墙上";
    
	traverseFolder(path, of, tf_handler);
	//批量一次写入剩下的文件
	if (gVideoSimilarityBuf.nCurPos >= 1) {
		const int nConsecutiveVideoNum = gCamOnLivingRoomWallExperValue.nConsecutiveVideoNum;
		string strTmp;
		//两遍循环扫描缓存中的视频，第一遍将防“误删”的视频标记出来
		for (int nPos = 1; nPos <= gVideoSimilarityBuf.nCurPos; nPos++) {
			//防视频“误删”算法，具体参考struct experimentalValue中的说明
			//当前视频标记为不删除时（通过比较视频的相似度获得）
			if ((gVideoSimilarityBuf.videoSimilarity[nPos - 1] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold)
				&& (nPos >= nConsecutiveVideoNum)) {
				//逐渐减少比较步长nStep
				int nStep = nConsecutiveVideoNum - 1;
				while (nStep >= 1) {
					//距离当前视频之前的第nStep个视频也是不删除的情况时（通过比较视频的相似度获得）
					if (gVideoSimilarityBuf.videoSimilarity[(nPos - 1) - nStep] <= gCamOnLivingRoomWallExperValue.fSimilarityThreshold) {
						//将当前视频和“距离当前视频之前的第nStep个视频”之间的视频全部标记为不删除
						for (int i = 1; i <= nStep; i++) {
							gVideoSimilarityBuf.videoResult[nPos - 1 - i] = 0;
						}
						break;
					}
					nStep--;
				}
			}
		}
		//第二遍扫描缓存中的视频，将视频处理结果对应的write2FileBuf结构体写入文件
		for (int nPos = 0; nPos < gVideoSimilarityBuf.nCurPos; nPos++) {
			strTmp += gVideoSimilarityBuf.videoRecord[nPos];
			char chTmp[256];
			//chTmp将是否删除的标记写入
			sprintf(chTmp, ",%d", gVideoSimilarityBuf.videoResult[nPos]);
			strTmp = strTmp + chTmp + gsLRCF;
			if (1 == gVideoSimilarityBuf.videoResult[nPos]) {
				if (remove(gVideoSimilarityBuf.videoFileFullPath[nPos].c_str()))
					cout << "删除文件失败：" << gVideoSimilarityBuf.videoFileFullPath[nPos] << endl;
			}
		}

		of << strTmp;
		//cout << "strTmp:" << strTmp;
		of.flush();
		cout << "一次性写入" << gVideoSimilarityBuf.nCurPos << "个视频文件相似度提取结果" << endl;
		gVideoSimilarityBuf.nCurPos = 0;
	}
	of.close();

	//此函数等待按键，按键盘任意键就返回
	waitKey();
	
	return 0;
}

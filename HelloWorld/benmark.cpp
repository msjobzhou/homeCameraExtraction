#include "stdafx.h"

#include "benchmark.h"

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"  
#include "opencv/cv.hpp"

#include "thread_pool.hpp"

#include <vector>
#include <string>

#include <thread>

#include<memory>
#include<functional>

using namespace std;
using namespace cv;

double calculateRunTime(VOID_FUNC pVoidFunc) {
	double t = (double)getTickCount();  

	if (!pVoidFunc) return -1.0f;

	(*pVoidFunc)();
	
	return  ((double)getTickCount() - t) / getTickFrequency();  

}

void task1(){
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\10.mp4", 2, 1);
}

void task2(){
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\10.mp4", 2, 2);
}

void testCreateFileCapture2Threads(){
	thread thread1(task1);
	thread thread2(task2);

	thread1.join();
	thread2.join();
}
void testCreateFileCapture(){
	//CvCapture *capture = cvCreateFileCapture("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\10.mp4");
	//CvCapture *capture1 = cvCreateFileCapture("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\11.mp4");
	//CvCapture *capture2 = cvCreateFileCapture("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\12.mp4");
	//CvCapture *capture3 = cvCreateFileCapture("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\13.mp4");
	//CvCapture *capture4 = cvCreateFileCapture("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\14.mp4");

	//VideoCapture vc("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\10.mp4");
	//VideoCapture vc1("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\11.mp4");
	//VideoCapture vc2("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\12.mp4");
	//VideoCapture vc3("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\13.mp4");
	//VideoCapture vc4("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\14.mp4");

	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2018-01-02\\08\\10.mp4",1,1);
}
/*
	��������readVideoEveryXFramesInNthPart
	���ã�����Ƶ�ļ����ֳ�N��NSegments���ж�ȡ�� �˺�����ȡ��nth�Σ� ÿ����Ƶ�����ǰ���֡���ж�ȡ������ÿ��X��֡��ȡ
	Ĭ�϶�ȡ������Ƶ
*/
void readVideoEveryXFramesInNthPart(const char* fileFullPath, int NSegments, int nth, int period) {
	
	CvCapture *capture = cvCreateFileCapture(fileFullPath);
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	//��Ƶ�ļ���Ϊ�գ�ֱ�ӷ���-1
	if (capture == NULL)
	{
		return;
	}
	IplImage *frame;
	int nStart = int(numFrames / NSegments)*(nth-1);
	int nEnd = int(numFrames / NSegments)*nth-1;
	int nPos = nStart;
	while (nPos <= nEnd && cvGrabFrame(capture))
	{
		//0-based index of the frame to be decoded/captured next
		cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, nPos);
		frame = cvRetrieveFrame(capture);
		if (!frame)
			break;
		nPos += period;
	}
	cvReleaseCapture(&capture);

}

void test_read_video_thread_pool()
{
	thread_pool tp;
	std::function<void()> pVoidFunc = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\00.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc1 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\01.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc2 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\02.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc3 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\03.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc4 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\04.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc5 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\05.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc6 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\06.mp4", 1, 1, 50);
	std::function<void()> pVoidFunc7 = std::bind(readVideoEveryXFramesInNthPart, "C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\07.mp4", 1, 1, 50);

	
	tp.submit(pVoidFunc);
	tp.submit(pVoidFunc1);
	tp.submit(pVoidFunc2);
	tp.submit(pVoidFunc3);
	tp.submit(pVoidFunc4);
	tp.submit(pVoidFunc5);
	tp.submit(pVoidFunc6);
	tp.submit(pVoidFunc7);

	//return 0;
}

void test_read_video()
{
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\00.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\01.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\02.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\03.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\04.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\05.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\06.mp4", 1, 1);
	readVideoEveryXFramesInNthPart("C:\\��������Ƶ����\\����ǽ��\\2017-08-15\\11\\07.mp4", 1, 1);
}
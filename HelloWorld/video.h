#pragma once

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"  
#include "opencv/cv.hpp"

#include <vector>
#include <string>
using namespace std;

int readVideoSaveImg(const char* fileName, const char* filePath, const char* saveImgPath, int period, vector<string> &vImgPath);

int readVideo(const char* fileName, const char* filePath, int period, vector<IplImage*> &vImg);
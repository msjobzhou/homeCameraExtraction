#include "stdafx.h"

#define _CRT_SECURE_NO_DEPRECATE  

#include <cstdio>  
#include <vector>  
#include <iostream>  
#include <fstream>  
#include <cstring>  
#include <cstdlib>  
#include <cmath>  
#include <algorithm>  

#include "opencv\cv.h"  
#include "opencv2\core\core.hpp"  
#include "opencv2\highgui\highgui.hpp"  
#include "opencv2\imgproc\imgproc.hpp"  
#include "opencv2\contrib\contrib.hpp"  

using namespace std;
using namespace cv;
int main1(int argc, char * argv[])
{
	string dir_path = "D:\\Test\\";
	Directory dir;
	vector<string> fileNames = dir.GetListFiles(dir_path, "*.jpg", false);

	for (int i = 0; i < fileNames.size(); i++)
	{
		string fileName = fileNames[i];
		string fileFullName = dir_path + fileName;
		cout << "file name:" << fileName << endl;
		cout << "file paht:" << fileFullName << endl << endl;

		//Image processing  
		//Mat pScr;
		//pScr = imread(fileFullName, 1); //以文件名命名窗口   
		//imshow(fileName, pScr);
		//waitKey(1000);


	}

	//system("pause");  
	return EXIT_SUCCESS;
}

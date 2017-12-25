#pragma once

#include <string>  

#include <iostream> 
using namespace std;

typedef void(*traverseFolder_handler)(string &filePath, ofstream &outFile);

bool listFiles(string fileName);

void traverseFolder(string folderPath, ofstream &fout, traverseFolder_handler tf_handler);

int mkdirByLevel(const char* path);

int getFolderAndFilename(char* fullPath, char* folder, char* fileName);
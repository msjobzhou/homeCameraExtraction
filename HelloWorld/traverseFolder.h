#pragma once

#include "rapidxml/rapidxml.hpp"       
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include <string>  

#include <iostream> 
#include<tchar.h>

using namespace std;

typedef void(*traverseFolder_handler)(string &filePath, ofstream &outFile);

bool listFiles(string fileName);

void traverseFolder(string folderPath, ofstream &fout, traverseFolder_handler tf_handler);

void traverseFolderAndSave2xml(string folderPath, rapidxml::xml_node<>* node, rapidxml::xml_document<> &doc);

int mkdirByLevel(const char* path);

int getFolderAndFilename(char* fullPath, char* folder, char* fileName);
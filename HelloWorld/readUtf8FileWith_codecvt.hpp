#pragma once

#include <fstream>
#include <string>
#include <codecvt>
#include <iostream>

using namespace std;

void readUtf8FileWith_codecvt()
{
	std::wstring str = L"123,abc:我是谁！";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

	std::string narrowStr = conv.to_bytes(str);
	{
		std::ofstream ofs("d:\\test_中文.txt");			//文件是utf8编码
		ofs << narrowStr;
	}

	std::ifstream ifs("d:\\test_中文.txt");
	while (!ifs.eof())
	{
		string line;
		getline(ifs, line);
		wstring wb = conv.from_bytes(line);
		wcout.imbue(locale("chs"));			//更改区域设置 只为控制台输出显示
		wcout << wb << endl;
	}
}
#pragma once

#include <fstream>
#include <string>
#include <codecvt>
#include <iostream>

using namespace std;

void readUtf8FileWith_codecvt()
{
	std::wstring str = L"123,abc:����˭��";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

	std::string narrowStr = conv.to_bytes(str);
	{
		std::ofstream ofs("d:\\test_����.txt");			//�ļ���utf8����
		ofs << narrowStr;
	}

	std::ifstream ifs("d:\\test_����.txt");
	while (!ifs.eof())
	{
		string line;
		getline(ifs, line);
		wstring wb = conv.from_bytes(line);
		wcout.imbue(locale("chs"));			//������������ ֻΪ����̨�����ʾ
		wcout << wb << endl;
	}
}
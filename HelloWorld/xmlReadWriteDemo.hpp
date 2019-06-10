#pragma once

#include "rapidxml/rapidxml.hpp"       
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

void write_xml() 
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* rot = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='gb2312'"));
	doc.append_node(rot);
	rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "config", NULL);
	doc.append_node(node);

	for (int i = 0; i<5; i++)
	{
		rapidxml::xml_node<>* stu = doc.allocate_node(rapidxml::node_element, "student", NULL);
		node->append_node(stu);

		char t[256];
		sprintf(t, "%d", i);
		std::string itag = t;

		std::string strname = "test_" + itag;
		char* pname = doc.allocate_string(strname.c_str());

		rapidxml::xml_attribute<>* pAttrType1 = doc.allocate_attribute("name", pname);
		stu->append_attribute(pAttrType1);

		std::string strage = "河北省小山村" + itag;
		char* page = doc.allocate_string(strage.c_str());

		pAttrType1 = doc.allocate_attribute("adress", page);
		stu->append_attribute(pAttrType1);
	}
	std::string text;
	rapidxml::print(std::back_inserter(text), doc, 0);

	std::ofstream out("config_rapidxml_中文.xml");
	out << doc;
}

void read_xml()
{
	//setlocale(LC_ALL, ""); // 解决中文路径问题（fstream）
	rapidxml::file<> f("config_rapidxml_中文.xml");
	//setlocale(LC_ALL, "C");
	rapidxml::xml_document<> doc;

	//doc.parse<0>(f.data());不包括版本号以及编码
	doc.parse<rapidxml::parse_full>(f.data());

	rapidxml::xml_node<>* pRoot = doc.first_node();
	if (pRoot == NULL)
	{
		return;
	}
	pRoot = pRoot->next_sibling();//config节点

	for (rapidxml::xml_node<>* pExeElem = pRoot->first_node(); pExeElem != NULL; pExeElem = pExeElem->next_sibling())
	{
		std::string szDstType;
		rapidxml::xml_attribute<>* pAttrType = pExeElem->first_attribute("name");
		if (pAttrType != NULL)
		{
			szDstType = pAttrType->value();
		}
		if (szDstType.compare("test_1") == 0)
		{
			rapidxml::xml_attribute<>* pAttrType1 = pExeElem->first_attribute("adress");
			std::string strpath = "浙江省";
			char* pname = doc.allocate_string(strpath.c_str());
			pAttrType1->value(pname);
		}
	}

	std::string text;
	rapidxml::print(std::back_inserter(text), doc, 0);

	//setlocale(LC_ALL, ""); // 解决中文路径问题（fstream）=
	std::ofstream outfile("config2_rapidxml_中文.xml");
	//setlocale(LC_ALL, "C");

	outfile << doc;
}

void read_xml_node(rapidxml::xml_node<>* pNode)
{
	for (rapidxml::xml_node<>* pExeElem = pNode; pExeElem != NULL; pExeElem = pExeElem->next_sibling())
	{
		//打印该节点的名字
		std::cout << pExeElem->name();
		if (NULL == pExeElem->first_node())
			std::cout << ": " << pExeElem->value();
		//std::cout << std::endl;
		//遍历该节点的所有属性
		std::string szTmpAttr("");
		for (rapidxml::xml_attribute<> * attr = pExeElem->first_attribute();
			attr != NULL;
			attr = attr->next_attribute())
		{
			szTmpAttr.append(attr->name());//name() value()返回的字符串不会去掉首尾的空白字符  
			szTmpAttr.append(": ");
			szTmpAttr.append(attr->value());
			szTmpAttr.append(", ");
		}
		std::cout << szTmpAttr.c_str() << std::endl;
		//递归遍历该节点的所有子节点
		rapidxml::xml_node<>* pExeElemSon = pExeElem->first_node();
		//for (; pExeElemSon != NULL; pExeElemSon = pExeElemSon->next_sibling())
		//{
			read_xml_node(pExeElemSon);
			//std::cout << pExeElemSon->name() << std::endl;
		//}
	}
}

void test_read_xml()
{
	rapidxml::file<> f("config_rapidxml_中文.xml");
	//setlocale(LC_ALL, "C");
	rapidxml::xml_document<> doc;

	//doc.parse<0>(f.data());不包括版本号以及编码
	doc.parse<rapidxml::parse_full>(f.data());

	rapidxml::xml_node<>* pRoot = doc.first_node();
	if (pRoot == NULL)
	{
		return;
	}
	pRoot = pRoot->next_sibling();//config节点
	//pRoot = pRoot->first_node();
	read_xml_node(pRoot);
}
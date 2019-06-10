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

		std::string strage = "�ӱ�ʡСɽ��" + itag;
		char* page = doc.allocate_string(strage.c_str());

		pAttrType1 = doc.allocate_attribute("adress", page);
		stu->append_attribute(pAttrType1);
	}
	std::string text;
	rapidxml::print(std::back_inserter(text), doc, 0);

	std::ofstream out("config_rapidxml_����.xml");
	out << doc;
}

void read_xml()
{
	//setlocale(LC_ALL, ""); // �������·�����⣨fstream��
	rapidxml::file<> f("config_rapidxml_����.xml");
	//setlocale(LC_ALL, "C");
	rapidxml::xml_document<> doc;

	//doc.parse<0>(f.data());�������汾���Լ�����
	doc.parse<rapidxml::parse_full>(f.data());

	rapidxml::xml_node<>* pRoot = doc.first_node();
	if (pRoot == NULL)
	{
		return;
	}
	pRoot = pRoot->next_sibling();//config�ڵ�

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
			std::string strpath = "�㽭ʡ";
			char* pname = doc.allocate_string(strpath.c_str());
			pAttrType1->value(pname);
		}
	}

	std::string text;
	rapidxml::print(std::back_inserter(text), doc, 0);

	//setlocale(LC_ALL, ""); // �������·�����⣨fstream��=
	std::ofstream outfile("config2_rapidxml_����.xml");
	//setlocale(LC_ALL, "C");

	outfile << doc;
}

void read_xml_node(rapidxml::xml_node<>* pNode)
{
	for (rapidxml::xml_node<>* pExeElem = pNode; pExeElem != NULL; pExeElem = pExeElem->next_sibling())
	{
		//��ӡ�ýڵ������
		std::cout << pExeElem->name();
		if (NULL == pExeElem->first_node())
			std::cout << ": " << pExeElem->value();
		//std::cout << std::endl;
		//�����ýڵ����������
		std::string szTmpAttr("");
		for (rapidxml::xml_attribute<> * attr = pExeElem->first_attribute();
			attr != NULL;
			attr = attr->next_attribute())
		{
			szTmpAttr.append(attr->name());//name() value()���ص��ַ�������ȥ����β�Ŀհ��ַ�  
			szTmpAttr.append(": ");
			szTmpAttr.append(attr->value());
			szTmpAttr.append(", ");
		}
		std::cout << szTmpAttr.c_str() << std::endl;
		//�ݹ�����ýڵ�������ӽڵ�
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
	rapidxml::file<> f("config_rapidxml_����.xml");
	//setlocale(LC_ALL, "C");
	rapidxml::xml_document<> doc;

	//doc.parse<0>(f.data());�������汾���Լ�����
	doc.parse<rapidxml::parse_full>(f.data());

	rapidxml::xml_node<>* pRoot = doc.first_node();
	if (pRoot == NULL)
	{
		return;
	}
	pRoot = pRoot->next_sibling();//config�ڵ�
	//pRoot = pRoot->first_node();
	read_xml_node(pRoot);
}
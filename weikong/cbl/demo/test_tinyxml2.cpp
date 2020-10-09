#include "../src/cbl.h"
#include "main.h"

static void example1()
{
	tXMLDocument doc;

	tXMLDeclaration* declaration = doc.NewDeclaration();
	doc.InsertFirstChild(declaration);

	tXMLElement *root = doc.NewElement(utf16_to_utf8(L"¸ù½Úµã").c_str());
	doc.InsertEndChild(root);

	tXMLElement *alarmNode = doc.NewElement("AlarmFile");
	root->InsertEndChild(alarmNode);

	for (int i = 0; i < 2; i++) {
		string name(3, 0x30 + i+1);
		tXMLElement *fileNode = doc.NewElement("File");
		fileNode->SetAttribute("name", name.c_str());
		fileNode->SetAttribute("size", "123456");
		alarmNode->InsertEndChild(fileNode);
	}

	tXMLElement *datalogNode = doc.NewElement("DataLogFile");
	root->InsertEndChild(datalogNode);

	for (int i = 0; i < 2; i++) {
		string name(3, 0x30 + i+3);
		tXMLElement *fileNode = doc.NewElement("File");
		fileNode->SetAttribute("name", name.c_str());
		fileNode->SetAttribute("size", "123456");
		datalogNode->InsertEndChild(fileNode);
	}

	tXMLPrinter printer;
	doc.Print(&printer);
	string sXml;
	sXml.append(printer.CStr());
	printf("%s\n", sXml.c_str());

	doc.SaveFile("demo/data/a.xml");
}

void test_tinyxml2()
{
	example1();

	getchar();
}
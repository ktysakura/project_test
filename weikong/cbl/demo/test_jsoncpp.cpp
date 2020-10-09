#include "../src/cbl.h"
#include "main.h"

void readFileJson()
{
	Json::Reader reader;
	Json::Value root;

	ifstream in("demo/data/a.json", ios::binary);
 
	if (!in.is_open()) { 
		cout << "Error opening file\n"; 
		return; 
	}
 
	if (reader.parse(in, root)) {
		//读取根节点信息
		string name = root["name"].asString();
		int age = root["age"].asInt();
		bool sex_is_male = root["sex_is_male"].asBool();
	 
		cout << "My name is " << name << endl;
		cout << "I'm " << age << " years old" << endl;
		cout << "I'm a " << (sex_is_male ? "man" : "woman") << endl;
	 
		//读取子节点信息
		string partner_name = root["partner"]["partner_name"].asString();
		int partner_age = root["partner"]["partner_age"].asInt();
		bool partner_sex_is_male = root["partner"]["partner_sex_is_male"].asBool();
	 
		cout << "My partner's name is " << partner_name << endl;
		cout << (partner_sex_is_male ? "he" : "she") << " is "
			<< partner_age << " years old" << endl;
	 
		//读取数组信息
		cout << "Here's my achievements:" << endl;
		for (unsigned int i = 0; i < root["achievement"].size(); i++) {
			string ach = root["achievement"][i].asString();
			cout << ach << '\t';
		}
		cout << endl;
		cout << "Reading Complete!" << endl;
	} else {
		cout << "parse error\n" << endl;	
	}
 
	in.close();
}

void writeFileJson()
{
	Json::FastWriter write;
	Json::Value root;

	Json::Value item;
	Json::Value arrayObj;
	item["book"] = utf16_to_utf8(L"语言");
	item["food"] = "apple";
	item["music"] = "ddx";
	arrayObj.append(item);

	root["name"] = "dsw";
	root["age"]  = 18;
	root["like"] = arrayObj; 

	string str = root.toStyledString();
	std::cout << str << std::endl;

	std::ofstream of;
	of.open("demo/data/b.json");
	of << str;
	of.close();
}

void test_jsoncpp()
{
	readFileJson();
	writeFileJson();

	getchar();
}
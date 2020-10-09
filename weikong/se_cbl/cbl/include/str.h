#pragma warning(disable: 4786)
#if !defined (STR_H__20170522)
#define STR_H__20170522

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

bool isHexChar(char c);
bool isNumeric(const string &sInput);
bool isIp(const string& sInput);

char *lcase(char *szInput);
string lcase(const string& sInput);
char *ucase(char *szInput);
string ucase(const string& sInput);
char *removeCRLF(char *szLine);
string removeCRLF(const string& sLine);
char *trim(char *szInput);
string trim(const string& sInput, const string &charlist = "\x13\x20");
string rtrim(const string& sInput, const string &charlist = "\x13\x20");
string ltrim(const string& sInput, const string &charlist = "\x13\x20");
bool isEndWith(const string &str, const string &charlist);
string dirName(const string& sPath);
string baseName(const string& sPath);
string prefixName(const string& sFileName);
string suffixName(const string& sFileName);
string replaceall(const string& sInput,const string& sOld,const string& sNew);
string format(const char *format,...);
string formatPath(const string& sInputPath);

int split(const string& sInput,const string& sSeparator,vector<string> *pvec);
int split(const string& sInput,const string& sSeparator,bool bTrim,bool bFilterEmpty,vector<string> *pvec);
int findPatternStrings(const string& sInput,const string& sPatternBeg,const string& sPatternEnd,map<int,string> *pmapReturn);

wstring string2wstring(const string& sInput);
string wstring2string(const wstring& wsInput);

#endif //STR_H__20170522

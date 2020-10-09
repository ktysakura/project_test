#if !defined (STR_H__20170522)
#define STR_H__20170522

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace cbl {

//判断c是否是16进制字符
bool isHex(char c);

//判断szInput是否是16进制字符串
bool isHex(const char *szInput);

//判断szInput是否是16进制字符串
bool isHex(const string &sInput);

//判断szInput是否是纯数字字符串
bool isNumeric(const char *szInput);

//判断szInput是否是纯数字字符串
bool isNumeric(const string &sInput);

//把szInput字符串转成全小写
char* lcase(char *szInput);

//把szInput字符串转成全小写
string lcase(const string &sInput);

//把szInput字符串转成全大写
char* ucase(char *szInput);

//把szInput字符串转成全大写
string ucase(const string &sInput);

char *removeCRLF(char *szLine);
string removeCRLF(const string& sLine);

//去除szInput的左右空白字符
char* trim(char *szInput);

//去除szInput的左右空白字符
string trim(const string &sInput);

//返回路径字符串中的目录名称
string dirName(const string &sPath);

//返回路径字符串中的不带目录的名称
string baseName(const string &sPath);

//返回文件名，不带后缀
string prefixName(const string &sFileName);

//返回文件后缀名称
string suffixName(const string& sFileName);

//把sInput字符串中的所有sOld子串替换成sNew子串
string replaceall(const string &sInput, const string &sOld, const string &sNew);

//返回格式化字符串
string format(const char *fmt,...);

//把sInput字符串按sSeparator进行分割，填入pvec，成功返回分割的个数，失败返回负数
int split(const string &sInput, const string &sSeparator, vector<string> *pvec);

//把sInput字符串按sSeparator进行分割，填入pvec，成功返回分割的个数，失败返回负数，比上一个函数多了去除空白和过滤空串的选项
int split(const string &sInput, const string &sSeparator, bool bTrim, bool bFilterEmpty, vector<string> *pvec);

//split的反向操作，把多个子串合并成一个新的字符串
string concat(const vector<string> &vecInput, const string &sSeparator);

} 

#endif

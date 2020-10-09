#if !defined (ENCODE_H__20170930)
#define ENCODE_H__20170930

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>

using namespace std;

namespace cbl {
	
typedef enum {	
	ASCII,
	UTF8,
	GBK,
	UTF16,
	UNKNOWN
} CODE_FORMAT_T;

//判断是否是ascii码字符串
bool isAscii(const string &sInput);

//判断是否是utf8编码字符串
bool isUtf8(const string &sInput);

//判断是否是gbk编码字符串
bool isGbk(const string &sInput);

//获取字符串的编码格式
CODE_FORMAT_T getCodeFormat(const string &sInput);

//gbk转utf8
string gbk_to_utf8(const string &sInput);

//utf8转gbk
string utf8_to_gbk(const string &sInput);

//utf16转gbk
string utf16_to_gbk(const wstring &sInput);

//gbk转utf16
wstring gbk_to_utf16(const string &sInput);

//utf16转utf8
string utf16_to_utf8(const wstring &sInput);

//utf8转utf16
wstring utf8_to_utf16(const string &sInput);

//utf16转windows系统当前代码页编码格式
string utf16_to_acp(const wstring &sInput);

//把windows系统当前代码页编码格式转成utf16
wstring acp_to_utf16(const string &sInput);

}

#endif

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

//�ж��Ƿ���ascii���ַ���
bool isAscii(const string &sInput);

//�ж��Ƿ���utf8�����ַ���
bool isUtf8(const string &sInput);

//�ж��Ƿ���gbk�����ַ���
bool isGbk(const string &sInput);

//��ȡ�ַ����ı����ʽ
CODE_FORMAT_T getCodeFormat(const string &sInput);

//gbkתutf8
string gbk_to_utf8(const string &sInput);

//utf8תgbk
string utf8_to_gbk(const string &sInput);

//utf16תgbk
string utf16_to_gbk(const wstring &sInput);

//gbkתutf16
wstring gbk_to_utf16(const string &sInput);

//utf16תutf8
string utf16_to_utf8(const wstring &sInput);

//utf8תutf16
wstring utf8_to_utf16(const string &sInput);

//utf16תwindowsϵͳ��ǰ����ҳ�����ʽ
string utf16_to_acp(const wstring &sInput);

//��windowsϵͳ��ǰ����ҳ�����ʽת��utf16
wstring acp_to_utf16(const string &sInput);

}

#endif

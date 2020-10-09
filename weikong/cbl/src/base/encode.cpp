#include "encode.h"
#include <windows.h>

namespace cbl {

//自定义一个代码页的宏，表示utf16编码，windows内部没有定义这个代码页
#define MY_CP_UTF16	65535

bool isAscii(const string &sInput)
{
	int nSize = sInput.length();
	
	for (int i = 0; i < nSize; i++) {
		if ((unsigned char)sInput[i] & 0x80) {
			return false;	
		}
	}
	
	return true;	
}

bool isUtf8(const string &sInput)
{	
	int i, j, nBytes, nSize = sInput.length();
	
	i = 0;
	while (i < nSize) {
		unsigned char cMask = 0xFE;
		unsigned char cMaskValue = 0xFC;
		
		if (0 == ((unsigned char)sInput[i] & 0x80)) {
			i++;
			continue;
		}
			
		for (j = 0; j < 5; j++) {
			if (cMaskValue == ((unsigned char)sInput[i] & cMask)) {
				break;
			}
			cMask <<= 1;
			cMaskValue <<= 1;	
		}

		if (j >= 5) {
			return false;		
		}
		
		nBytes = 6 - j;
		if (i + nBytes > nSize) {
			return false;	
		}
		
		for (j = 1; j < nBytes; j++) {
			if (0x80 != ((unsigned char)sInput[i+j] & 0xC0)) {
				return false;	
			}
		}
		
		i += nBytes;
	}
	
	return true;
}

bool isGbk(const string &sInput)
{
	int i, nSize = sInput.length();
	
	i = 0;
	while (i < nSize) {
		if (0 == ((unsigned char)sInput[i] & 0x80)) {
			i++;
			continue;	
		}
		
		if (((unsigned char)sInput[i] < 0x81) || 
			((unsigned char)sInput[i] > 0xFE)) {
			return false;
		}
		
		if (i + 2 > nSize) {
			return false;	
		}
	
		if (((unsigned char)sInput[i+1] < 0x40) || 
			((unsigned char)sInput[i+1] > 0xFE)) {
			return false;
		}
		
		i += 2;
	}
	
	return true;
}

CODE_FORMAT_T getCodeFormat(const string &sInput)
{
	if (true == isAscii(sInput)) {
		return ASCII;	
	}
	
	if (true == isUtf8(sInput)) {
		return UTF8;	
	}
	
	if (true == isGbk(sInput)) {
		return GBK;
	}

	return UNKNOWN;
}

//获取字符编码对应的代码页
int pageCodeFormat(CODE_FORMAT_T format, UINT &nPageCode)
{
	
	switch (format) {
	case ASCII:
		nPageCode = CP_ACP;
		break;

	case UTF8:
		nPageCode = CP_UTF8;
		break;

	case UTF16:
		nPageCode = MY_CP_UTF16; 
		break;

	case GBK:
		nPageCode = 54936; //GB18030, starting with Windows Vista
		break;

	default:
		return -1;
	}
	
	return 0;
}

//实际的转码函数
int encode(CODE_FORMAT_T codeFrom, CODE_FORMAT_T codeTo, const string &sInput, string &sOutput)
{
	wchar_t *pWBuffer = NULL;
	char *pMBuffer = NULL;
	int nLen, nExitCode = 0;
	UINT nCodePageFrom, nCodePageTo;
	
	/* get code page */
	if ((pageCodeFormat(codeFrom, nCodePageFrom) < 0) || (pageCodeFormat(codeTo, nCodePageTo) < 0)) {
		return -1;	
	}
	
	/* if same code page */
	if (nCodePageFrom == nCodePageTo) {
		sOutput = sInput;
		return 0;
	}

	if (nCodePageFrom == MY_CP_UTF16) {
		/* malloc pWBuffer */
		if (NULL == (pWBuffer = (wchar_t *)malloc(sInput.length()))) {
			return -2;
		}

		memcpy(pWBuffer, sInput.data(), sInput.length());
	} else {
		/* convert wchar to mchar */
		if (0 == (nLen = MultiByteToWideChar(nCodePageFrom, 0, sInput.c_str(), -1 , NULL, 0))) {
			return -3;
		}

		/* malloc pWBuffer */
		if (NULL == (pWBuffer = (wchar_t *)malloc(nLen * sizeof(wchar_t)))) {
			return -4;
		}

		/* convert mchar to wchar */
		MultiByteToWideChar(nCodePageFrom, 0, sInput.c_str(), -1, pWBuffer, nLen);

		if (nCodePageTo == MY_CP_UTF16) {
			sOutput.assign((char *)pWBuffer, wcslen(pWBuffer) * sizeof(wchar_t));
			nExitCode = 0;
			goto __EXIT_ENCODE;
		}
	}

	/* calc multi bytes */
	if (0 == (nLen = WideCharToMultiByte(nCodePageTo, 0, pWBuffer, -1 , NULL, 0, NULL, NULL))) {
		nExitCode = -5;
		goto __EXIT_ENCODE;
	}
	
	/* malloc pMBuffer */
	if (NULL == (pMBuffer = (char *)malloc(nLen))){
		nExitCode = -6;
		goto __EXIT_ENCODE;	
	}
		
	/* convert wchar to mchar */
	WideCharToMultiByte(nCodePageTo, 0, pWBuffer, -1, pMBuffer, nLen, NULL, NULL);
	
	/* assign output */
	sOutput = pMBuffer;

__EXIT_ENCODE:

	/* free pMBuffer */
	if(pMBuffer){
		free (pMBuffer);		
	}

	/* free pWBuffer */
	if(pWBuffer){
		free (pWBuffer);		
	}

	return nExitCode;
}

string utf8_to_gbk(const string &sInput)
{
	string sOutput;
	
	if (encode(UTF8, GBK, sInput, sOutput) < 0) {
		return string();
	}
	
	return sOutput;
}

string gbk_to_utf8(const string &sInput)
{
	string sOutput;
	
	if (encode(GBK, UTF8, sInput, sOutput) < 0) {
		return string();
	}
	
	return sOutput;
}

string utf16_to_gbk(const wstring &sInput)
{
	string sOutput;
	string sData;

	//把wstring的输入转成string
	sData.assign((const char *)sInput.data(), (sInput.length() + 1) * sizeof(wchar_t));

	if (encode(UTF16, GBK, sData, sOutput) < 0) {
		return string();
	}

	return sOutput;
}

wstring gbk_to_utf16(const string &sInput)
{
	wstring sOutput;
	string sData;

	if (encode(GBK, UTF16, sInput, sData) < 0) {
		return wstring();
	}

	sOutput.assign((const wchar_t *)sData.data(), sData.length() / sizeof(wchar_t));
	return sOutput;
}

string utf16_to_utf8(const wstring &sInput)
{
	string sOutput;
	string sData;

	//把wstring的输入转成string
	sData.assign((const char *)sInput.data(), (sInput.length() + 1) * sizeof(wchar_t));
	
	if (encode(UTF16, UTF8, sData, sOutput) < 0) {
		return string();
	}

	return sOutput;
}

wstring utf8_to_utf16(const string &sInput)
{
	wstring sOutput;
	string sData;

	if (encode(UTF8, UTF16, sInput, sData) < 0) {
		return wstring();
	}
	
	sOutput.assign((const wchar_t *)sData.data(), sData.length() / sizeof(wchar_t));
	return sOutput;
}

string utf16_to_acp(const wstring &sInput)
{
	string sOutput;
	string sData;

	//把wstring的输入转成string
	sData.assign((const char *)sInput.data(), (sInput.length() + 1) * sizeof(wchar_t));

	if (encode(UTF16, ASCII, sData, sOutput) < 0) {
		return string();
	}

	return sOutput;
}

wstring acp_to_utf16(const string &sInput)
{
	wstring sOutput;
	string sData;

	if (encode(ASCII, UTF16, sInput, sData) < 0) {
		return wstring();
	}

	sOutput.assign((const wchar_t *)sData.data(), sData.length() / sizeof(wchar_t));
	return sOutput;
}

} 

#include "str.h"
#include "dir.h"
#include <stdarg.h>
#include <io.h>

namespace cbl {

bool isHex(char c)
{
	return (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) ? true : false;
}

bool isHex(const char *szInput)
{
	int nSize;
	
	if ((NULL == szInput) || ((nSize = (int)strlen(szInput)) <= 0)) {
		return false;
	}

	for (int i = 0; i < nSize; i++) {
		if (!isHex(szInput[i])) {
			return false;
		}
	}
	
	return true;
}

bool isHex(const string &sInput)
{
	return isHex(sInput.c_str());
}

bool isNumeric(const char *szInput)
{
	int nLen, i = 0;
	
	if ((NULL == szInput) || ((nLen = (int)strlen(szInput)) <= 0)) {
		return false;
	}
	
	if ('-' == szInput[0]) {
		i++;
	}

	for (; i < nLen; i++) {
		if ((szInput[i] < '0') || (szInput[i] > '9')) {
			return false;
		}
	}

	return true;
}

bool isNumeric(const string &sInput)
{
	return isNumeric(sInput.c_str());
}

char* trim(char *szInput)
{
	if (NULL != szInput) {
		int i, nLen;
		
		//È¥³ý×ó±ßµÄ¿Õ°××Ö·û
		nLen = (int)strlen(szInput);
		for (i = 0; i < nLen; i++) {
			if ((0x09 != szInput[i]) && (0x20 != szInput[i])) {
				if (i > 0) {
					memcpy(szInput, &szInput[i], nLen - i);
					szInput[nLen - i] = '\0';
				}			
				break;
			}	
		}

		//È«²¿¶¼ÊÇ¿Õ°××Ö·û
		if (i >= nLen) {
			szInput[0] = '\0';
		} else {
			//È¥³ýÓÒ±ßµÄ¿Õ°××Ö·û
			nLen = (int)strlen(szInput);
			for (i = nLen - 1; i >= 0; i--) {
				if ((0x09 != szInput[i]) && (0x20 != szInput[i])) {
					if (i < nLen - 1) {
						szInput[i + 1] = '\0';
					}			
					break;
				}	
			}
		}
	}
	
	return szInput;
}

string trim(const string &sInput)
{
	int i, nLen;
	string sOutput = sInput;
	
	//È¥³ý×ó±ßµÄ¿Õ°××Ö·û
	nLen = (int)sOutput.length();
	for (i = 0; i < nLen; i++){
		if ((0x09 != sOutput[i]) && (0x20 != sOutput[i])) {
			if (i > 0) {
				sOutput = sOutput.substr(i, nLen - i);
			}			
			break;
		}	
	}

	//È«²¿¶¼ÊÇ¿Õ°××Ö·û
	if (i >= nLen) {
		sOutput = "";
	}	
	else{
		//È¥³ýÓÒ±ßµÄ¿Õ°××Ö·û
		nLen = (int)sOutput.length();
		for (i = nLen - 1; i >= 0; i--) {
			if ((0x09 != sOutput[i]) && (0x20 != sOutput[i])) {
				if (i < nLen - 1) {
					sOutput = sOutput.substr(0, i + 1);
				}			
				break;
			}	
		}
	}
	
	return sOutput;
}

char* lcase(char *szInput) 
{
	if (szInput) {
		int i, nLen = (int)strlen(szInput);
		
		for (i = 0; i < nLen; i++){
			szInput[i] = tolower(szInput[i]);
		}	
	}
	
	return szInput;
}

string lcase(const string &sInput)
{
	string sOutput = sInput;
	int i, nLen = (int)sOutput.length();
	
	for (i = 0; i < nLen; i++) {
		sOutput[i] = tolower(sOutput[i]);
	}

	return sOutput;
}

char* ucase(char *szInput)
{
	if (szInput) {
		int i, nLen = (int)strlen(szInput);
		
		for (i = 0; i < nLen; i++) {
			szInput[i] = toupper(szInput[i]);
		}	
	}
	
	return szInput;
}

string ucase(const string& sInput)
{
	string sOutput = sInput;
	int i, nLen = (int)sOutput.length();
	
	for (i = 0; i < nLen; i++) {
		sOutput[i] = toupper(sOutput[i]);
	}

	return sOutput;
}

char *removeCRLF(char *szLine)
{
	if (NULL != szLine) {
		int i, nSize = (int)strlen(szLine);	

		for (i = nSize - 1; i >= 0; i--) {
			if (('\r' != szLine[i]) && ('\n' != szLine[i])) {				
				break;	
			}	
		}
		szLine[i + 1] = '\0';
	}
	
	return szLine;
}

string removeCRLF(const string &sLine)
{
	string sOutput = sLine;
	int i, nSize = (int)sOutput.length();
	
	for (i = nSize - 1; i >= 0; i--) {
		if (('\r' != sOutput[i]) && ('\n' != sOutput[i])) {			
			break;	
		}	
	}
	sOutput.erase(i+1);
	return sOutput;
}

string dirName(const string &sPath)
{
	int i, nLen = (int)sPath.length();
	
	for (i = nLen - 1; i >= 0; i--) {
		if (('\\' == sPath[i]) || ('/' == sPath[i])) {
			break;
		}
	}
	
	return sPath.substr(0, i);
}

string baseName(const string &sPath)
{
	int i,nLen = (int)sPath.length();
	
	for (i = nLen - 1; i >= 0; i--) {
		if (('\\' == sPath[i]) || ('/' == sPath[i])) {
			break;
		}
	}
	
	return sPath.substr(i + 1);
}

string prefixName(const string &sFileName)
{
	int nPos;
	string sBaseName;
	
	sBaseName = baseName(sFileName);
	if ((nPos = (int)sBaseName.find_last_of('.')) < 0) {
		return sBaseName;
	}
	
	return sBaseName.substr(0, nPos);
}

string suffixName(const string &sFileName)
{
	int nPos;
	string sBaseName;
	
	sBaseName = baseName(sFileName);
	if ((nPos = (int)sBaseName.find_last_of('.')) < 0) {
		return "";
	}
	
	return sBaseName.substr(nPos);
}

string replaceall(const string &sInput, const string &sOld, const string &sNew) 
{
	string sOutput;
	int nPosFind, nPosBeg = 0, nLenOld = (int)sOld.length();
	
	while ((nPosFind = (int)sInput.find(sOld, nPosBeg)) >= 0) {
		if (nPosFind - nPosBeg > 0) {
			sOutput += sInput.substr(nPosBeg, nPosFind - nPosBeg);
		}

		sOutput += sNew;
		nPosBeg = nPosFind + nLenOld; 
	}
	
	if (nPosBeg < (int)sInput.length()) {
		sOutput += sInput.substr(nPosBeg);
	}
	
	return sOutput;
}

string format(const char *fmt,...)
{
	va_list vl;
	char szBuffer[8192] = {0};
	
	va_start(vl, fmt);
	vsprintf_s(szBuffer, sizeof(szBuffer), fmt, vl);
	va_end(vl);
	
	return szBuffer;	
}

int split(const string &sInput, const string &sSeparator, vector<string> *pvec)
{
	int nLen, nFindPos, nBegPos = 0;
	
	if (((nLen = (int)sSeparator.length()) <= 0) || (NULL == pvec)) {
		return -1;	
	}
	pvec->clear();
	
	while ((nFindPos = (int)sInput.find(sSeparator, nBegPos)) >= 0) {
		pvec->push_back(sInput.substr(nBegPos, nFindPos - nBegPos));
		nBegPos = nFindPos + nLen;
	}

	if (nBegPos < (int)sInput.size()) {
		pvec->push_back(sInput.substr(nBegPos));
	} else {
		pvec->push_back("");
	}
	
	return int(pvec->size());
}

int split(const string &sInput, const string &sSeparator, bool bTrim, bool bFilterEmpty, vector<string> *pvec)
{
	if (split(sInput, sSeparator, pvec) < 0) {
		return -1;
	}

	if (bTrim || bFilterEmpty) {
		vector<string>::iterator it = pvec->begin();

		while (it != pvec->end()) {
			if (bTrim){
				*it = trim(*it);
			}
			
			if (bFilterEmpty && (it->length() <= 0)) {
				it = pvec->erase(it);
			} else {
				++it;
			}
		}
	}
	return int(pvec->size());
}

string concat(const vector<string> &vecInput, const string &sSeparator)
{
	if (vecInput.size() <= 0) {
		return string();
	}
	
	vector<string>::const_iterator vit = vecInput.begin();
	string sOutput = *vit++;
	
	for (; vit != vecInput.end(); ++vit) {
		sOutput += sSeparator;
		sOutput += *vit;
	}
	
	return sOutput;		
}

} 

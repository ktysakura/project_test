#if !defined(MD5_UTIL_H__20190810)
#define MD5_UTIL_H__20190810
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "md5.h"
using namespace std;

namespace cbl{

int md5Buffer(const unsigned char *pBuffer, int nSize, unsigned char szDigest[16]);
int md5Buffer(const unsigned char *pBuffer, int nSize, string& sDigest);
int md5String(const string& sInput, unsigned char szDigest[16]);
int md5String(const string& sInput, string& sDigest);

int md5File(const string& sFilePath,long long llOffset,long long llSize,unsigned char szDigest[16],long long *pllMd5Size = NULL);

int md5File(const string& sFilePath,long long llOffset,long long llSize,string& sDigest,long long *pllMd5Size = NULL);
int md5File(const string& sFilePath, unsigned char szDigest[16]);
int md5File(const string& sFilePath, string& sDigest);

} //namespace cbl

#endif //MD5_UTIL_H__20190810

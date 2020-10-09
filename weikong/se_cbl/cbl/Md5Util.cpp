
#define  _CRT_SECURE_NO_WARNINGS

#include "Md5Util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "buffer.h"


#if defined(WIN32) || defined (_WIN32)

#else

#include <unistd.h>

#endif

namespace cbl{

/* md5 buffer
**/
int md5Buffer(const unsigned char *pBuffer, int nSize, unsigned char szDigest[16]){

	MD5_CTX ctx;

	/* check params */
	if(NULL == szDigest){
		return -1;
	}

	MD5Init(&ctx);
	if((NULL != pBuffer) && (nSize > 0)){
		MD5Update(&ctx, (unsigned char *)pBuffer, nSize);
	}	
	MD5Final(&ctx, szDigest);

	return 0;
}

/* md5 buffer
**/
int md5Buffer(const unsigned char *pBuffer, int nSize, string& sDigest){
	
	int nRtn;
	unsigned char szDigest[16] = {0};
	
	if((nRtn = md5Buffer(pBuffer, nSize, szDigest)) < 0){
		return nRtn;
	}
	sDigest.assign((const char*)szDigest, sizeof(szDigest));
#if 0
	if(hexBuffer(szDigest, 16, false, sDigest) < 0){
		return -127;
	}
#endif
	return 0;
}

/* md5 string
**/
int md5String(const string& sInput, unsigned char szDigest[16]){
	return md5Buffer((unsigned char *)sInput.c_str(), sInput.size(), szDigest);
}

/* md5 string
**/
int md5String(const string& sInput, string& sDigest){
	return md5Buffer((unsigned char *)sInput.c_str(), sInput.size(), sDigest);
}

/* md5 file
 */
int md5File(const string& sFilePath,long long llOffset,long long llSize,unsigned char szDigest[16],long long *pllMd5Size){
	
	MD5_CTX ctx;
	FILE *fp = NULL;
	struct stat statFile;
	unsigned char *pBuffer = NULL;
	long long llMd5Size, llRemainSize;
	int nNeedBytes, nReadBytes, nRetCode = 0, nBufferSize = (4L << 20);

	/* check offset */
	if ((llOffset < 0) || (NULL == szDigest)){
		nRetCode = -1;
		goto __EXIT_MD5FILE;
	}

	/* stat file */
	
	if (0 != stat(sFilePath.c_str (), &statFile)){
		nRetCode = -2;
		goto __EXIT_MD5FILE;
	}

	/* get and check size */
	if (llSize > 0)	{
		llMd5Size = min(statFile.st_size - llOffset, llSize);
	}
	else{
		llMd5Size = statFile.st_size - llOffset;
	}

	/* open file */
	if (NULL == (fp = fopen(sFilePath.c_str(), "rb"))){
		nRetCode = -3;
		goto __EXIT_MD5FILE;
	}

	/* seek for offset */
	if (0 != fseek(fp, (long)llOffset, SEEK_SET)){
		nRetCode = -4;
		goto __EXIT_MD5FILE;
	}
	
	/* init md5 */
	MD5Init (&ctx);
	
	/* if file size > 0 */
	if (statFile.st_size > 0){

		/* malloc buffer */
		if (statFile.st_size < nBufferSize){
			nBufferSize = statFile.st_size;
		}
		if (NULL == (pBuffer = (unsigned char *)malloc(nBufferSize))){
			nRetCode = -5;
			goto __EXIT_MD5FILE;
		}
		
		/* cals md5 */
		llRemainSize = llMd5Size;
		while(llRemainSize > 0){
			/* read file */
			nNeedBytes = (int)min (llRemainSize, (long long)nBufferSize);
			if ((nReadBytes = (int)fread(pBuffer, 1, nNeedBytes, fp)) <= 0){
				nRetCode = -6;
				goto __EXIT_MD5FILE;
			}

			/* update md5 */
			MD5Update (&ctx, pBuffer, nReadBytes);

			/* sync remain size */
			llRemainSize -= nReadBytes;
		}
	}
	
	/* final md5 */
	MD5Final (&ctx, szDigest);

	/* assign file size */
	if (pllMd5Size){
		*pllMd5Size = llMd5Size;
	}

__EXIT_MD5FILE:
	
	/* free buffer */
	if (pBuffer){
		free (pBuffer);
	}	

	/* close file */
	if (fp){
		fclose (fp);
	}	

	return nRetCode;
}

/* md5 file
 */
int md5File(const string& sFilePath,long long llOffset,long long llSize,string& sDigest,long long *pllMd5Size){
	
	int nRtn;
	unsigned char szDigest[16] = {0};
	
	if((nRtn = md5File(sFilePath, llOffset, llSize, szDigest, pllMd5Size)) < 0){
		return nRtn;
	}
	
	if(hexBuffer(szDigest, 16, false, sDigest) < 0){
		return -127;
	}
	
	return 0;
}

/* md5 file
**/
int md5File(const string& sFilePath, unsigned char szDigest[16]){
	
	return md5File(sFilePath, 0, -1, szDigest, NULL);
}

/* md5 file
**/
int md5File(const string& sFilePath, string& sDigest){
	
	return md5File(sFilePath, 0, -1, sDigest, NULL);
}



} //namespace cbl

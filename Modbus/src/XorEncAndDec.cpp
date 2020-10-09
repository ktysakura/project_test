#include "XorEncAndDec.h"
#include <iostream>
#define  _ENC_AND_DEC 1

int CXorEncAndDec::EncData(const void *pin, void *pout, const string &sKeySeed,  unsigned int len)
{
	return this->EncAndDec((const unsigned char *)pin, (unsigned char *)pout, sKeySeed, len);
}

int CXorEncAndDec::DecData(const void *pin, void *pout, const string &sKeySeed, unsigned int len)
{
	return this->EncAndDec((const unsigned char *)pin, (unsigned char *)pout, sKeySeed, len);	
}
int CXorEncAndDec::EncData(const string& sInput, string& sOutput,const string & sKeySeed)
{
	sOutput.resize(sInput.size());
	return this->EncData(sInput.c_str(), const_cast<char*>(sOutput.c_str()), sKeySeed, sInput.size());
}
int CXorEncAndDec::DecData(const string& sInput, string& sOutput,const string & sKeySeed)
{
	sOutput.resize(sInput.size());
	return this->DecData(sInput.c_str(), const_cast<char*>(sOutput.c_str()), sKeySeed, sInput.size());
}
int CXorEncAndDec::EncAndDec(const unsigned char *pin, unsigned char  *pout, const string &sKeySeed, unsigned int len)
{
#if _ENC_AND_DEC
	int nRet = 0;
	unsigned char commKey[ENCRY_KEY_LEN] = { 0 };
	if (NULL == pin || NULL== pout || sKeySeed.size() != 16)
	{
		nRet = -1;
		goto __EXIT_CODE;
	}

	if (0 != len % 16)
	{
		nRet = -2;
		goto __EXIT_CODE;
	}

	if (false == XorEncrypt(pin, pout, (const unsigned char*)sKeySeed.c_str(), len))
	{
		nRet = -3;
		goto __EXIT_CODE;
	}

__EXIT_CODE:

	return nRet;
#else
	memcpy(pout, pin, len);
	return 0;
#endif
}

/**
  * @brief      由密钥种子转换成密钥
  * @note       
  * @attention  None
  * @param      实际密钥
  * @param      key seed
  * @retval     true/false
  */
bool CXorEncAndDec::GetCommuiKey(unsigned char *pCommuiKey, const unsigned char *pKeySeed)
{
	int i;
	unsigned char PublicKey[] = "5&4Fe@d_J]*:@W5N";
	unsigned char Temp;
	if (NULL == pCommuiKey || NULL == pKeySeed)
	{
		return false;
	}

	for (i = 0; i < ENCRY_KEY_LEN; i++)
	{
		Temp = pKeySeed[PublicKey[i] % ENCRY_KEY_LEN] % ENCRY_KEY_LEN;
		pCommuiKey[i] = (pKeySeed[Temp] * PublicKey[(i + 5) % ENCRY_KEY_LEN] + PublicKey[Temp]) ^ pKeySeed[ (Temp + i) % ENCRY_KEY_LEN];
	}

	return true;
}

/**
  * @brief      异或加解密
  * @note       可从中间加解密的模式，每块16字节
  * @param      pin：要加密/解密的目标
  * @param      pout：加密/解密后的密文
  * @param      pkey：密钥-16字节
  * @param      len：加密长度-16的倍数
  * @retval     ture/false
  */
bool CXorEncAndDec::XorEncrypt(const unsigned char *pin, unsigned char *pout, const unsigned char *pkey,unsigned int len)
{
	unsigned int i, j;
	unsigned char k;

	if (NULL == pin || NULL == pout || NULL == pkey)
	{
		return false;
	}

	if (0 != len % ENCRY_KEY_LEN)
	{
		return false;
	}

	k = 0;
	for (j = 0; j < (len/ENCRY_KEY_LEN); j++)
	{
		for (i = 0; i < ENCRY_KEY_LEN; i++)
		{
			pout[i + j*ENCRY_KEY_LEN] = (pin[i + j*ENCRY_KEY_LEN] ^ pkey[i]) ^ (0xff - k);
			k++;
		}
	}

	return true;
}

#pragma once
#include <string>
using namespace std;
#define ENCRY_KEY_LEN 16


class CXorEncAndDec
{
public:
	int EncData(const void *pin, void *pout, const string &sKeySeed, unsigned int len);
	int DecData(const void *pin, void *pout, const string &sKeySeed, unsigned int len);
	int EncData(const string& sInput, string& sOutput,const string & sKeySeed);
	int DecData(const string& sInput, string& sOutput,const string & sKeySeed);
	/**
	  * @brief      由密钥种子转换成密钥
	  * @note       
	  * @attention  None
	  * @retval     False 指针异常
	  * @param      实际密钥
	  * @param      密钥种子
	  */
	bool GetCommuiKey(unsigned char *pCommuiKey, const unsigned char *pKeySeed);
private:
	int EncAndDec(const unsigned char *pin, unsigned char  *pout, const string &sKeySeed,unsigned int len);
	/**
	  * @brief      异或加解密
	  * @note       可从中间加解密的模式，每块16字节
	  * @param      pin：要加密/解密的目标
	  * @param      pout：加密/解密后的密文
	  * @param      pkey：密钥-16字节
	  * @param      len：加密长度-16的倍数
	  * @retval     TRUE/FALSE
	  */
	bool XorEncrypt(const unsigned char *pin, unsigned char *pout, const unsigned char *pkey,unsigned int len);
};

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
	  * @brief      ����Կ����ת������Կ
	  * @note       
	  * @attention  None
	  * @retval     False ָ���쳣
	  * @param      ʵ����Կ
	  * @param      ��Կ����
	  */
	bool GetCommuiKey(unsigned char *pCommuiKey, const unsigned char *pKeySeed);
private:
	int EncAndDec(const unsigned char *pin, unsigned char  *pout, const string &sKeySeed,unsigned int len);
	/**
	  * @brief      ���ӽ���
	  * @note       �ɴ��м�ӽ��ܵ�ģʽ��ÿ��16�ֽ�
	  * @param      pin��Ҫ����/���ܵ�Ŀ��
	  * @param      pout������/���ܺ������
	  * @param      pkey����Կ-16�ֽ�
	  * @param      len�����ܳ���-16�ı���
	  * @retval     TRUE/FALSE
	  */
	bool XorEncrypt(const unsigned char *pin, unsigned char *pout, const unsigned char *pkey,unsigned int len);
};

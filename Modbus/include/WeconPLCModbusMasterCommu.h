#if !defined(RTU_MODBUS_MASTER_COMMU_PLC_H__20190326)
#define RTU_MODBUS_MASTER_COMMU_PLC_H__20190326
#include "ModbusMasterCommu.h"
#include "WeconPLCHeader.h"
#include "XorEncAndDec.h"
#include <map>
#include "buffer.h"
typedef struct _tagOnCommFunc
{
	void *arg;
	int (*on_commu_run)(int, int, void *);
	int (*on_commu_begin)(void*);
	int (*on_commu_end)(void*);
	_tagOnCommFunc()
	{
		on_commu_run = NULL;
		on_commu_begin = NULL;
		on_commu_end = NULL;
	}
}ONCOMMFUNC_T;

class CWeconPLCModbusMasterCommu : public CModbusMasterCommu
{

public:
	void setHandle(ONCOMMFUNC_T onCommuFunc);
	/*����дλ*/
	int writeBitInRegister(int nAddress, int bit, bool bOn, int nTimeout = 1000);
	/*д���ӹ�����*/
	int writeSubCode(int nSubCode/*in*/, int nTimeout = 1000/*in*/);
	int writeFile(int fileNo, const unsigned char *pBuffer,  unsigned long nFileSize, int nTimeout, bool bExWrite);
	/*����չģ��*/
	int readModule(int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/, vector<int>& vecValue/*out*/, int nTimeout =1000);
	/*д��չģ��*/
	int writeModule(int nModuleNo, int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
	/*дʱ��*/
	int writeClock(const struct tm &tm_Time, int nTimeout = 1000);
	/*��ʱ��*/
	int readClock(struct tm &tm_Time, int nTimeout = 1000);
	/*�����ļ�*/
	int downloadFile(int nPreCode, PLC_FILE_NO fileNo, const list<FILE_ATTRIBUTE_T> &listFile, int nLimitSize, const string &sPassword, bool bExWrite, int nTimeout = 1000);
	/*�ϴ��ļ�*/
	int uploadFile(PLC_FILE_NO fileNo, list<string> &listFileData, const string &sPassword, int nTimeout = 1000);
	/*��ȡWCP��hash*/
	int readWcpHash(string& sHash);
	/*У��WCP��hash*/
	int checkWcpHash(const string& sHash);
	int readVersion(int nVersion);
	int writeMultiRegistersOnWork(int nAddress, const vector<int>& vecValue, int nCurProgess, int nTotoalProgress);
	int writeMultiCoilsOnWork(int nAddress, const vector<int>& vecValue, int nCurProgess, int nTotoalProgress);
	int readMultiRegistersOnWork(int nAddress, int nCount, vector<int> &vecValue, int nCurProgess, int nTotoalProgress);
	int readMultiCoilsOnWork(int nAddress, int nCount, vector<int> &vecValue, int nCurProgess, int nTotoalProgress);
public:
	/*д������*/
	int writePassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout = 1000);
	/*��ʱ����*/
	int decTemporary(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout = 1000);
	/*�������*/
	int checkPassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int &nCode, int nTimeout = 1000);
	/*�����д��֤*/
	int verifyPassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int &nCode, int nTimeout = 1000);
	/*ɾ������*/
	int removePassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout = 1000);
	/*��ȡ������Ϣ*/
	int readEncryptInfo(int nIndex, int nCount, map<PASSWORD_KEY_TYPE, bool> &mapEncryptInfo, int nTimeout = 1000);
	/*�Ƴ���ʱ����*/
	int removeDecTemporary(PASSWORD_KEY_TYPE keyType, int nTimeout = 1000);
	
private:
	/*��ȡ��Կ����*/
	int getKeySeed(string& sKeySeed);
	/*��ȡ�������*/
	int readRandomSeed(string &sRandomSeed, int nTimeout = 1000);
	/*�����ʱ����*/
	int removeDecTemporary(const vector<char>& vecKey, int nTimeout = 1000);
	int operatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, int nTimeout = 1000);
	int operatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, int &nCode, int nTimeout);
	int readPLCInfo(int nSubCode, int nIndex, int nCount, string& sData, int nTimeout);
private:
	int writeInterior(int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
	int readInterior(int nAddress, int nCount, vector<int>& vecValue, int nTimeout = 1000);
	int readFileData(int nFileNo/*in*/, int nRecordNo/*in*/, int nFileSize/*in*/, string& sData/*out*/, int nTimeout = 1000);
	int writeFileData(int nFileNo/*in*/, int nRecordNo/*in*/,  unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout /*in*/, bool bExWrite);
	int writeFileRecordEx(int nFileNo/*in*/, int nRecordNo/*in*/, const unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout = 1000 /*in*/);

private:
	void makeupData(string& sData, int demicals);
	CXorEncAndDec m_xor;
	string m_sKeySeed;
	ONCOMMFUNC_T m_commFunc;
};

#endif
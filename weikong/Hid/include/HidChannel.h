#ifndef CHIDCHANNEL_H_20190916
#define CHIDCHANNEL_H_20190916

#include <string>
#include <list>
#include <vector>

#ifdef WIN32
#include "Hid.h"
#else
#include <string.h>
#include <pthread.h> 
#include "Hid.h"
#endif

#include "hid_namespace.h"
#include "HidPackage.h"
#include "SemaphoreEx.h"
#include "SemaphorePool.h"

using namespace cbl;
using namespace std;

#define TIMEOUT_FRAME_WRITE				1000	//send�����е���֡��д��ʱʱ��
#define TIMEOUT_FRAME_WAIT				3000	//send�����е���֡�ȴ�Ӧ����źŵƳ�ʱʱ��
#define TIMEOUT_REPLY_WRITE				1000	//reply�����е�д��ʱʱ��
#define TIMEOUT_POLL_READ				500   	//�߳���ѯ���ĳ�ʱʱ��
#define TIMEOUT_WAIT_THREAD_EXIT		4000	//�ȴ����߳��˳��ĳ�ʱʱ��



USBHID_BEGIN_NAMESPACE

class CHidChannel;

//���ڽ���������Ļص�����
//typedef int (*pf_onRecvRequest)(CHidChannel *pHidChannel, CPackage &package);

//���ڽ������ݴ���Ļص�����
//typedef int (*pf_onRecvData)(CHidChannel *pHidChannel, CPackage &package);

//usbhidͨ����
class  CHidChannel
{
public:
	CHidChannel(unsigned char initSessionId = 0);
	virtual ~CHidChannel();
#ifdef WIN32
	//��hid�豸
	int open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number = NULL);
#else
	int open(const char *hidName, int mode);
#endif
	bool isOpen();
	//������������ͨ��response����Ӧ������
	int sendAndRecv(unsigned char *data, size_t length, int nHopeSize, string &response, int nTimeout);
	//ֹͣusbhidͨ���������߳̽���
	int close();
private:
	//��ȡ�Ựid
	unsigned char nextSessionId(int nCount = 1);
	int read(unsigned char *pRecv, size_t length, string &sReponse);
	int makeRequest(unsigned char *data, size_t length, list<CHidPackage> &listRequest);
private:
	bool m_bOpened;
	unsigned char m_sessionId;
	int m_nOutReportLen;
	int m_nInputReportLen;
	bool m_bStop;
	CHid m_hid;
};


USBHID_END_NAMESPACE




#endif
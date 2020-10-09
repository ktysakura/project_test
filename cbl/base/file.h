#if !defined(FILE_H__20190129)
#define FILE_H__20190129

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;

namespace cbl {
	
//���б������ļ�ʱ�Ļص�����
typedef int (*PFONNEWLINE)(const char *szLine, void *parameter);

//��ȡ�ļ������д浽list���ɹ�����������ʧ�ܷ��ظ���
int readTextFile(const string &sFileName, list<string> *pvecLine);

//��ȡ�ļ������д浽list���ɹ�����������ʧ�ܷ��ظ���
int readTextFile(FILE *fp, list<string> *pvecLine);

//��ȡ�ļ������е��ûص�����
int readTextFile(const string &sFileName, PFONNEWLINE pfOnNewLine, void *parameter);

//��ȡ�ļ������е��ûص�����
int readTextFile(FILE *fp, PFONNEWLINE pfOnNewLine, void *parameter);

//��ָ���Ķ�дģʽ���ļ�������λ��ָ����λ��
int locateFile(const string &sFilePath, bool bEnableRead, bool bEnableWrite, int nOffset, int nWhence, int &fd);

//��ָ�����ȵ�����д�뵽fd
int writeToFile(const void *pBuffer, size_t nSize, int fd);

//��ָ�����ȵ�����д�뵽fd
int writeToFile(const string &sData, int fd);

//��ȡ�ļ���С
long long getFileSize(int fd);

//��ȡ�ļ���С
long long getFileSize(const string &sFilePath);

//���ļ���ָ��λ�ö�ȡһ����С�����ݵ�buf�У��ɹ����ض�ȡ�Ĵ�С��������Ҫ�����ͷ�ppBuffer��ʧ�ܷ��ظ���
int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, unsigned char **ppBuffer, int nLimitSize = (16 << 20));

//���ļ���ָ��λ�ö�ȡһ����С�����ݵ�string�У��ɹ����ض�ȡ�Ĵ�С��ʧ�ܷ��ظ���
int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, string &sData, int nLimitSize = (16 << 20));

//��ȡ�ļ��������ݵ�buf�У��ɹ����ض�ȡ�Ĵ�С��������Ҫ�����ͷ�ppBuffer��ʧ�ܷ��ظ���
int loadFromFile(const string &sFilePath, unsigned char **ppBuffer, int nLimitSize = (16 << 20));

//��ȡ�ļ��������ݵ�string�У����ɹ����ض�ȡ�Ĵ�С��ʧ�ܷ��ظ���
int loadFromFile(const string &sFilePath, string &sData, int nLimitSize = (16 << 20));

//��buf�е�����д��ָ�����ļ�
int saveToFile(const void *pBuffer, int nSize, const string &sFilePath);

//��string�е�����д�뵽ָ�����ļ�
int saveToFile(const string &sData, const string &sFilePath);

} 

#endif 

#pragma once
#include <string>
using namespace std;

enum PLC_FILE_NO
{
	PLC_BINWCP_NO = 1,  //bin+wcp�ļ���
	PLC_PARAM_NO,      //PLC�����ļ���
	PLC_USER_PWD_NO,   //�û������ļ���
	PLC_COMMENT_NO,    //ע���ļ���
	FILE_NUMMAX
};

enum PLC_FILE_TYPE
{
	PLC_TYPE_BIN,        //bin����
	PLC_TYPE_PARAM,      //PLC��������
	PLC_TYPE_USER_PWD,   //�û���������
	PLC_TYPE_COMMENT,    //ע������
	PLC_TYPE_WCP,        //WCP����
	PLC_TYPE_NUMMAX
};

#define KB  (1 << 10)
#define MD5_LEN       (0x10)						     //md5��У�鳤��
#define COMM_BUFFER_SIZE (0x80)							 //�����ϴ�ͨѶ�Ļ������̶���С
#define MALLOC_BUFFER_SIZE (4L << 20)				     //һ�ο��ٵĿռ��С������������,Ŀǰ4MӦ�ù���

#define LOADADDR_BIN  (0x20200000)						 //bin�ļ����ص�ַ
#define LOADADDR_NONE (0xFFFFFFFF)						 //��ʾ���ļ�û�м��ص�ַ

#define COMMENT_SIZE_LIMIT (128 << 10)					 //ע�������������ش�С(byte)
#define COMMENT_SIZE_LIMIT_KB (COMMENT_SIZE_LIMIT >> 10) //ע�������������ش�С(kb)
#define BIN_SIZE_LIMIT  (512 << 10)						 //bin�����������ش�С(byte)
#define BIN_SIZE_LIMIT_KB (BIN_SIZE_LIMIT >> 10)		 //bin�����������ش�С(kb)
#define BINWCP_SIZE_LIMIT (876 << 10)					 //bin+wcp�����������ش�С(byte)

#define ERROR_OUTOFRANGE_BIN (-1000)					 //bin�ļ�������������ش�С������
#define ERROR_OUTOFRANGE	 (-1001)					 //�����ļ�������ش�С�����Ĵ�����
#define UNCHECK_LIMIT          (-1)					     //���������ش�С

typedef struct tagFILE_HEAD_T
{
	union{
		struct{
			unsigned char MagicNumber[16];    //��ͷ�̶��ַ���
			unsigned int HeadLen;             //ͷ����
			unsigned short HeadVer;           //�ļ�ͷ�汾
			unsigned short FilewareVersion;   //�̼��汾
			unsigned short LadderLowVersion;  //���Ҫ��Ĺ̼��汾
			unsigned short Type;              //�ļ���
			unsigned short LoaderNumber;      //��������
			unsigned short Save;              //���뱣��
			unsigned char  FileHash[MD5_LEN]; //�����ļ�hash
			unsigned int AllFileSize;         //�ļ���С =  �ļ���0���MD5����
			unsigned char WcpHash[MD5_LEN];   //wcp hash
		};
		unsigned char Save2[128-16];          //Ԥ����Ϣ 16���ֽ�Ϊ����hashУ�� 
	};	
}FILE_HEAD_T;


typedef struct tagSECTION_T
{
	unsigned int Offset;      //�ļ�ƫ����
	unsigned int ExecAddr;    //���ص�ַ
	unsigned int SecFileSize; //�ļ���С
	unsigned int Save;        //Ԥ����Ϣ
}SECTION_T;

typedef struct tagFILE_ATTRIBUTE_T
{
	unsigned int nExecAddr;    //���ص�ַ
	string sData;              //��������
	PLC_FILE_TYPE type;    
	tagFILE_ATTRIBUTE_T()
	{
		Reset();
	}

	~tagFILE_ATTRIBUTE_T()
	{
		Reset();
	}

	void Reset()
	{
		nExecAddr = LOADADDR_NONE;
		sData.clear();
	}
}FILE_ATTRIBUTE_T;


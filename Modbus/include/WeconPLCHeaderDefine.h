#pragma once
#include <string>
using namespace std;

enum PLC_FILE_NO
{
	PLC_BINWCP_NO = 1,  //bin+wcp文件号
	PLC_PARAM_NO,      //PLC参数文件号
	PLC_USER_PWD_NO,   //用户密码文件号
	PLC_COMMENT_NO,    //注释文件号
	FILE_NUMMAX
};

enum PLC_FILE_TYPE
{
	PLC_TYPE_BIN,        //bin类型
	PLC_TYPE_PARAM,      //PLC参数类型
	PLC_TYPE_USER_PWD,   //用户密码类型
	PLC_TYPE_COMMENT,    //注释类型
	PLC_TYPE_WCP,        //WCP类型
	PLC_TYPE_NUMMAX
};

#define KB  (1 << 10)
#define MD5_LEN       (0x10)						     //md5的校验长度
#define COMM_BUFFER_SIZE (0x80)							 //下载上传通讯的缓冲区固定大小
#define MALLOC_BUFFER_SIZE (4L << 20)				     //一次开辟的空间大小，来报存数据,目前4M应该够用

#define LOADADDR_BIN  (0x20200000)						 //bin文件加载地址
#define LOADADDR_NONE (0xFFFFFFFF)						 //表示该文件没有加载地址

#define COMMENT_SIZE_LIMIT (128 << 10)					 //注释最大允许的下载大小(byte)
#define COMMENT_SIZE_LIMIT_KB (COMMENT_SIZE_LIMIT >> 10) //注释最大允许的下载大小(kb)
#define BIN_SIZE_LIMIT  (512 << 10)						 //bin最大允许的下载大小(byte)
#define BIN_SIZE_LIMIT_KB (BIN_SIZE_LIMIT >> 10)		 //bin最大允许的下载大小(kb)
#define BINWCP_SIZE_LIMIT (876 << 10)					 //bin+wcp最大允许的下载大小(byte)

#define ERROR_OUTOFRANGE_BIN (-1000)					 //bin文件若超过最大下载大小错误码
#define ERROR_OUTOFRANGE	 (-1001)					 //其他文件最大下载大小超过的错误码
#define UNCHECK_LIMIT          (-1)					     //无需检查下载大小

typedef struct tagFILE_HEAD_T
{
	union{
		struct{
			unsigned char MagicNumber[16];    //开头固定字符串
			unsigned int HeadLen;             //头长度
			unsigned short HeadVer;           //文件头版本
			unsigned short FilewareVersion;   //固件版本
			unsigned short LadderLowVersion;  //最低要求的固件版本
			unsigned short Type;              //文件号
			unsigned short LoaderNumber;      //加载数量
			unsigned short Save;              //对齐保留
			unsigned char  FileHash[MD5_LEN]; //整个文件hash
			unsigned int AllFileSize;         //文件大小 =  文件补0后的MD5长度
			unsigned char WcpHash[MD5_LEN];   //wcp hash
		};
		unsigned char Save2[128-16];          //预留信息 16个字节为最后的hash校验 
	};	
}FILE_HEAD_T;


typedef struct tagSECTION_T
{
	unsigned int Offset;      //文件偏移量
	unsigned int ExecAddr;    //加载地址
	unsigned int SecFileSize; //文件大小
	unsigned int Save;        //预留信息
}SECTION_T;

typedef struct tagFILE_ATTRIBUTE_T
{
	unsigned int nExecAddr;    //加载地址
	string sData;              //数据类型
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


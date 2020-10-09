#ifndef __CSERIALPORT_DEFINE_H_2019_9_17_
#define __CSERIALPORT_DEFINE_H_2019_9_17_


namespace cbl{

/**
 * @brief the read and write serial port mode enum 读写串口模式
 * 
 */
/**
 * @brief the BaudRate enum 波特率
 * 
 */
enum BaudRate
{
	BaudRate1200 = 1200,		///< 1200
	BaudRate2400 = 2400,		///< 2400
	BaudRate4800 = 4800,		///< 4800
	BaudRate9600 = 9600,		///< 9600 recommend 推荐
	BaudRate19200 = 19200,		///< 19200
	BaudRate38400 = 38400,		///< 38400
	BaudRate115200 = 115200,	///< 115200
};

/**
 * @brief the DataBits enum 数据位
 * @warning 
 * by QextSerialPort:\n
 * 5 data bits cannot be used with 2 stop bits 5位数据位不能使用2位停止位\n
 * 1.5 stop bits can only be used with 5 data bits 1.5位停止位不能使用5位数据位\n
 * 8 data bits cannot be used with space parity on POSIX systems POSIX系统8位数据位不能使用0校验
 * @warning windows Number of bits/byte, 4-8 windows数据位范围为4 - 8
 * 
 */
enum DataBits
{
	DataBits5 = 5,			///< 5 data bits 5位数据位
	DataBits6 = 6,			///< 6 data bits 6位数据位
	DataBits7 = 7,			///< 7 data bits 7位数据位
	DataBits8 = 8			///< 8 data bits 8位数据位
};

/**
 * @brief the Parity enum 校验位
 * @warning windows 0-4=None,Odd,Even,Mark,Space
 * 
 */
enum Parity
{
	ParityNone  = 0,			///< No Parity 无校验
	ParityOdd   = 1,			///< Odd Parity 奇校验
	ParityEven  = 2,			///< Even Parity 偶校验
	ParityMark  = 3,            ///< Mark Parity 1校验
	ParitySpace = 4,			///< Space Parity 0校验	 
};

/**
 * @brief the StopBits enum 停止位
 * @warning 1.5 stop bit only for the Windows platform 1.5位停止位仅对windows有效
 * @warning windows 0,1,2 = 1, 1.5, 2
 * 
 */
enum StopBits
{
	StopOne = 0,				///< 1 stop bit 1位停止位
	StopOneAndHalf = 1,			///< 1.5 stop bit 1.5位停止位 - This is only for the Windows platform
	StopTwo = 2					///< 2 stop bit 2位停止位
};

/**
 * @brief the FlowConctrol enum 流控制
 * 
 */
enum FlowConctrol
{
	FlowNone = 0,		///< No flow control 无流控制
	FlowHardware = 1,	///< Hardware(RTS / CTS) flow control 硬件流控制
	FlowSoftware = 2	///< Software(XON / XOFF) flow control 软件流控制
};

/**
 * @brief the SerialPort error code 串口错误代码
 * 
 */
enum SerialPortError
{
	NoError,							///< No error occurred 没有错误
	DeviceNotFoundError,				///< device not found 未找到设备
	PermissionError,					///< permission error 权限错误
	OpenError,							///< open error 打开串口错误
	ParityError,						///< parity error 校验位错误
	FramingError,						///< 
	BreakConditionError,				///< 
	WriteError,							///< write error 写数据错误
	ReadError,							///< read error 读数据错误
	ResourceError,						///< 
	UnsupportedOperationError,			///< 
	UnknownError,						///< unknown error 未知错误
	TimeoutError,						///< time out error 超时
	NotOpenError						///< not open serial port error 串口未打开	
};

typedef struct tagSERIALPORT_INFO_T{

	BaudRate     baudRate;
	Parity       parity;
	DataBits     dataBit;
	StopBits     stopbits;
	FlowConctrol flowConctrol;

	tagSERIALPORT_INFO_T()
	{
		reset();
	}

	~tagSERIALPORT_INFO_T()
	{
		reset();
	}

	void reset()
	{	
		baudRate = BaudRate115200;
		parity = ParityNone;
		dataBit = DataBits8;
		stopbits = StopOne;
		flowConctrol = FlowNone;
	}

}SERIALPORT_INFO_T;

}
#endif //__CSERIALPORT_GLOBAL_H__
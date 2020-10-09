#if !defined(MODBUS_DEFINE_H__20190416)
#define MODBUS_DEFINE_H__20190416

/* define function code
*/
typedef enum{
	FUNCTION_CODE_READ_COILS			= 0x01,
	FUNCTION_CODE_READ_REGISTERS		= 0x03,
	FUNCTION_CODE_WRITE_SINGLE_COIL		= 0x05,
	FUNCTION_CODE_WRITE_SINGLE_REGISTER	= 0x06,
	FUNCTION_CODE_WRITE_COILS			= 0x0F,
	FUNCTION_CODE_WRITE_REGISTERS		= 0x10,
	FUNCTION_CODE_READ_FILERECORD		= 0x14,
	FUNCTION_CODE_WRITE_FILERECORD		= 0x15,
	FUNCTION_CODE_MOTION_CONTROL		= 0x41,
	FUNCTION_CODE_MAX
}FUNCTION_CODE_T;

/* define macros
**/
#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#elif defined(__LINUX__) || defined(LINUX)
#	define BYTE(x)			((x) & 0xFF)
#	define HIBYTE(x)        BYTE(((x) >> 8)) 
#	define LOBYTE(x)		BYTE(x)
#endif

#define COMM_RW_MAX_SIZE          (0x0080)  /*0x0080 = 128*/
#define WRITE_REGISTERS_MAX_COUNT (0x0078)  /*0x0078 = 120*/
#define READ_REGISTERS_MAX_COUNT  (0x007D)  /*0x007D = 125*/
#define READ_COILS_MAX_COUNT      (0x07D0)  /*0x07D0 = 2000*/
#define WRITE_COILS_MAX_COUNT     (0x07B0)  /*0x07B0 = 1968*/

#define modbus_status_frame_not_equal   -100
#define modbus_status_station_error     -10
#define modbus_status_stack_not_enough  -11
#define modbus_status_ok                0

//自定义错误内容
#define MODBUS_ERROR_OFFSET                    1000
#define MODBUS_NOT_HAVE_FILE                   (-(MODBUS_ERROR_OFFSET + 0x13)) //        文件内容为空
#endif //MODBUS_DEFINE_H__20190416
#if !defined(BIG_ENDIAN_H__20180822)
#define BIG_ENDIAN_H__20180822

#include "DataType.h"

namespace cbl {
	
//返回pBuffer指针开始的连续2个字节按大端模式表示的整数值，成功返回0，失败返回负数
int getBigendianValueU16(const void *pBuffer, U16 *result);

//把uValue按大端模式写入pBuffer指针开始的连续2个字节位置，成功返回0，失败返回负数
int setBigendianValueU16(U16 uValue, void *pBuffer);

//返回pBuffer指针开始的连续4个字节按大端模式表示的整数值，成功返回0，失败返回负数
int getBigendianValueU32(const void *pBuffer, U32 *result);

//把uValue按大端模式写入pBuffer指针开始的连续4个字节位置，成功返回0，失败返回负数
int setBigendianValueU32(U32 uValue, void *pBuffer);

//返回pBuffer指针开始的连续8个字节按大端模式表示的整数值，成功返回0，失败返回负数
int getBigendianValueU64(const void *pBuffer, U64 *result);

//把uValue按大端模式写入pBuffer指针开始的连续8个字节位置，成功返回0，失败返回负数
int setBigendianValueU64(U64 uValue, void *pBuffer);

} 

#endif

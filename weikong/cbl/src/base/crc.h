#if !defined (CRC_H__20141105153052)
#define CRC_H__20141105153052

namespace cbl {
	
//crc32函数
unsigned long int crc32(const void *inBuf, size_t bytes);

//crc64函数
unsigned long long int crc64(const void *inBuf, size_t bytes);

//crc16查表法函数
int crc16_table(const void *inBuf, size_t bytes);

//crc16直接计算法函数
int crc16_calc(const void *inBuf, size_t bytes);

//逐字节异或函数
unsigned char calc_xor(const void *pbuf, int len);

} 

#endif 

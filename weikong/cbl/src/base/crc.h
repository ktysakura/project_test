#if !defined (CRC_H__20141105153052)
#define CRC_H__20141105153052

namespace cbl {
	
//crc32����
unsigned long int crc32(const void *inBuf, size_t bytes);

//crc64����
unsigned long long int crc64(const void *inBuf, size_t bytes);

//crc16�������
int crc16_table(const void *inBuf, size_t bytes);

//crc16ֱ�Ӽ��㷨����
int crc16_calc(const void *inBuf, size_t bytes);

//���ֽ������
unsigned char calc_xor(const void *pbuf, int len);

} 

#endif 

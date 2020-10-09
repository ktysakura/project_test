#if !defined(BIG_ENDIAN_H__20180822)
#define BIG_ENDIAN_H__20180822

#include "DataType.h"
namespace cbl{

	U16 getBigendianValueU16(unsigned char *pBuffer);
	bool setBigendianValueU16(U16 uValue,unsigned char *pBuffer);
	U32 getBigendianValueU32(unsigned char *pBuffer);
	bool setBigendianValueU32(U32 uValue,unsigned char *pBuffer);
	U64 getBigendianValueU64(unsigned char *pBuffer);
	bool setBigendianValueU64(U64 uValue,unsigned char *pBuffer);

}
#endif //BIG_ENDIAN_H__20180822

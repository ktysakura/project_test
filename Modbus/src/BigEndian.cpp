#include "BigEndian.h"
namespace cbl{
	/* get value for U16
	 */
	U16 getBigendianValueU16(unsigned char *pBuffer){
		
		if(pBuffer){
			return ((*pBuffer) << 8) 
					| (*(pBuffer + 1));
		}
		
		return 0;
	}

	/* set value for U16
	 */
	bool setBigendianValueU16(U16 uValue,unsigned char *pBuffer){
		
		if(pBuffer){
			*pBuffer 		= ((uValue >> 8) & 0xFF);
			*(pBuffer + 1) 	= (uValue & 0xFF);
			
			return true;
		}
		
		return false;
	}

	/* get value for U32
	 */
	U32 getBigendianValueU32(unsigned char *pBuffer){
		if(pBuffer){
			return ((*pBuffer) << 24) 
				| ((*(pBuffer + 1)) << 16) 
				| ((*(pBuffer + 2)) << 8) 
				| (*(pBuffer + 3));
		}
		
		return 0;	
	}

	/* set value for U32
	 */
	bool setBigendianValueU32(U32 uValue,unsigned char *pBuffer){
		
		if(pBuffer){
			*pBuffer 		= ((uValue >> 24) & 0xFF);
			*(pBuffer + 1) 	= ((uValue >> 16) & 0xFF);
			*(pBuffer + 2) 	= ((uValue >> 8) & 0xFF);
			*(pBuffer + 3) 	= (uValue & 0xFF);
			
			return true;
		}
		
		return false;
	}

	/* get valur for U64
	 */
	U64 getBigendianValueU64(unsigned char *pBuffer){
		
		U64 uValue = 0;
		
		if(pBuffer){		
			U8 *pHead = pBuffer;
			
			for(int i=0; i<8; i++){
				uValue <<= 8;
				uValue |= *pHead++;
			}
		}
		
		return uValue;	
	}

	/* set valur for U64
	 */
	bool setBigendianValueU64(U64 uValue,unsigned char *pBuffer){
		
		if(pBuffer){
			U8 *pHead = pBuffer;
			
			for(int i=0; i<8; i++){
				*pHead++ = ((uValue >> ((7 - i) << 3)) & 0xFF);
			}
			
			return true;
		}
		
		return false;	
	}

}
#include "buffer.h"

/* hex buffer
**/
int hexBuffer(const void *pBuffer,int nSize,bool bReverse,string& sHex){
 
 int i;
 char szHex[16] = {0};
 const int c_nSize = sizeof(szHex) - 1;
 const unsigned char *pHead = (const unsigned char *)pBuffer;
 
 if(NULL == pBuffer){
  return -1;
 }
 
 sHex.clear();
 if(bReverse){
  pHead += (nSize - 1);
  for(i=0; i<nSize; i++){
   sprintf_s(szHex, c_nSize, "%02x", ((*pHead) & 0xFF));
   sHex.append(szHex);
   pHead--;
  }  
 }
 else{
  for(i=0; i<nSize; i++){
   sprintf_s(szHex, c_nSize, "%02x", ((*pHead) & 0xFF));
   sHex.append(szHex);
   pHead++;
  }  
 }
 
 return sHex.size();
} 
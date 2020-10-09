#if !defined(DATA_TYPE_H__20180822)
#define DATA_TYPE_H__20180822

#include <stdio.h>
#include <stdlib.h>

#define U8 	unsigned char
#define U16 unsigned short
#define U32 unsigned int
#define U64 unsigned long long

#ifndef _WIN32
	#define SOCKET 				int
	#define INVALID_SOCKET 		(-1)
	#define SOCKET_ERROR		(-1)	
	#define __int64 			long long
	#define closesocket(sock) 	::close(sock)
	#define _atoi64(str)		strtoll(str, NULL, 10)
	
	typedef struct sockaddr_in SOCKADDR_IN;
#endif //_WIN32

#endif //DATA_TYPE_H__20180822

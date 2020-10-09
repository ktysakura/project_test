#if !defined(DATA_TYPE_H__20180822)
#define DATA_TYPE_H__20180822

#define U8 	unsigned char
#define U16 unsigned short
#define U32 unsigned int
#define U64 unsigned long long

#define cbl_max(a,b) ((a) > (b) ? (a) : (b))
#define cbl_min(a,b) ((a) < (b) ? (a) : (b))

#define ARRAY_LEN(x)	(sizeof(x) / sizeof(x[0]))

#endif

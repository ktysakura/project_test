#include "md5.h"

namespace cbl {

#define F(x, y, z)   ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)   ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)   ((x) ^ (y) ^ (z))
#define I(x, y, z)   ((y) ^ ((x) | ~(z)))

#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
	(a) += (b);

#define SET(n) 		(*(unsigned int *)&ptr[(n) * 4])
#define GET(n) 		SET(n)

const void* CMd5::md5Body(md5_t *ctx, const void *data, unsigned long size)
{
	const unsigned char *ptr;
	unsigned int a, b, c, d;
	unsigned int saved_a, saved_b, saved_c, saved_d;

	ptr = (const unsigned char*)data;

	a = ctx->a;
	b = ctx->b;
	c = ctx->c;
	d = ctx->d;

	do {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;

		STEP(F, a, b, c, d, SET(0),  0xd76aa478, 7)
			STEP(F, d, a, b, c, SET(1),  0xe8c7b756, 12)
			STEP(F, c, d, a, b, SET(2),  0x242070db, 17)
			STEP(F, b, c, d, a, SET(3),  0xc1bdceee, 22)
			STEP(F, a, b, c, d, SET(4),  0xf57c0faf, 7)
			STEP(F, d, a, b, c, SET(5),  0x4787c62a, 12)
			STEP(F, c, d, a, b, SET(6),  0xa8304613, 17)
			STEP(F, b, c, d, a, SET(7),  0xfd469501, 22)
			STEP(F, a, b, c, d, SET(8),  0x698098d8, 7)
			STEP(F, d, a, b, c, SET(9),  0x8b44f7af, 12)
			STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
			STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
			STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
			STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
			STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
			STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)
			STEP(G, a, b, c, d, GET(1),  0xf61e2562, 5)
			STEP(G, d, a, b, c, GET(6),  0xc040b340, 9)
			STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
			STEP(G, b, c, d, a, GET(0),  0xe9b6c7aa, 20)
			STEP(G, a, b, c, d, GET(5),  0xd62f105d, 5)
			STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
			STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
			STEP(G, b, c, d, a, GET(4),  0xe7d3fbc8, 20)
			STEP(G, a, b, c, d, GET(9),  0x21e1cde6, 5)
			STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
			STEP(G, c, d, a, b, GET(3),  0xf4d50d87, 14)
			STEP(G, b, c, d, a, GET(8),  0x455a14ed, 20)
			STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
			STEP(G, d, a, b, c, GET(2),  0xfcefa3f8, 9)
			STEP(G, c, d, a, b, GET(7),  0x676f02d9, 14)
			STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)
			STEP(H, a, b, c, d, GET(5),  0xfffa3942, 4)
			STEP(H, d, a, b, c, GET(8),  0x8771f681, 11)
			STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
			STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23)
			STEP(H, a, b, c, d, GET(1),  0xa4beea44, 4)
			STEP(H, d, a, b, c, GET(4),  0x4bdecfa9, 11)
			STEP(H, c, d, a, b, GET(7),  0xf6bb4b60, 16)
			STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23)
			STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
			STEP(H, d, a, b, c, GET(0),  0xeaa127fa, 11)
			STEP(H, c, d, a, b, GET(3),  0xd4ef3085, 16)
			STEP(H, b, c, d, a, GET(6),  0x04881d05, 23)
			STEP(H, a, b, c, d, GET(9),  0xd9d4d039, 4)
			STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11)
			STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
			STEP(H, b, c, d, a, GET(2),  0xc4ac5665, 23)
			STEP(I, a, b, c, d, GET(0),  0xf4292244, 6)
			STEP(I, d, a, b, c, GET(7),  0x432aff97, 10)
			STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
			STEP(I, b, c, d, a, GET(5),  0xfc93a039, 21)
			STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
			STEP(I, d, a, b, c, GET(3),  0x8f0ccc92, 10)
			STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
			STEP(I, b, c, d, a, GET(1),  0x85845dd1, 21)
			STEP(I, a, b, c, d, GET(8),  0x6fa87e4f, 6)
			STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
			STEP(I, c, d, a, b, GET(6),  0xa3014314, 15)
			STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
			STEP(I, a, b, c, d, GET(4),  0xf7537e82, 6)
			STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
			STEP(I, c, d, a, b, GET(2),  0x2ad7d2bb, 15)
			STEP(I, b, c, d, a, GET(9),  0xeb86d391, 21)

			a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	ctx->a = a;
	ctx->b = b;
	ctx->c = c;
	ctx->d = d;

	return ptr;
}

void CMd5::md5Init(md5_t *ctx)
{
	ctx->a = 0x67452301;
	ctx->b = 0xefcdab89;
	ctx->c = 0x98badcfe;
	ctx->d = 0x10325476;

	ctx->lo = 0;
	ctx->hi = 0;
}

void CMd5::md5Update(md5_t *ctx, const void *data, unsigned long size)
{
	unsigned int saved_lo;
	unsigned long used, free;

	saved_lo = ctx->lo;
	if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
		ctx->hi++;
	ctx->hi += size >> 29;
	used = saved_lo & 0x3f;

	if (used) {
		free = 64 - used;
		if (size < free) {
			memcpy(&ctx->buffer[used], data, size);
			return;
		}

		memcpy(&ctx->buffer[used], data, free);
		data = (unsigned char *)data + free;
		size -= free;
		md5Body(ctx, ctx->buffer, 64);
	}

	if (size >= 64) {
		data = md5Body(ctx, data, size & ~(unsigned long)0x3f);
		size &= 0x3f;
	}

	memcpy(ctx->buffer, data, size);
}

void CMd5::md5Final(unsigned char *result, md5_t *ctx)
{
	unsigned long used, free;
	used = ctx->lo & 0x3f;
	ctx->buffer[used++] = 0x80;
	free = 64 - used;

	if (free < 8) {
		memset(&ctx->buffer[used], 0, free);
		md5Body(ctx, ctx->buffer, 64);
		used = 0;
		free = 64;
	}

	memset(&ctx->buffer[used], 0, free - 8);

	ctx->lo <<= 3;
	ctx->buffer[56] = ctx->lo;
	ctx->buffer[57] = ctx->lo >> 8;
	ctx->buffer[58] = ctx->lo >> 16;
	ctx->buffer[59] = ctx->lo >> 24;
	ctx->buffer[60] = ctx->hi;
	ctx->buffer[61] = ctx->hi >> 8;
	ctx->buffer[62] = ctx->hi >> 16;
	ctx->buffer[63] = ctx->hi >> 24;
	md5Body(ctx, ctx->buffer, 64);
	result[0] = ctx->a;
	result[1] = ctx->a >> 8;
	result[2] = ctx->a >> 16;
	result[3] = ctx->a >> 24;
	result[4] = ctx->b;
	result[5] = ctx->b >> 8;
	result[6] = ctx->b >> 16;
	result[7] = ctx->b >> 24;
	result[8] = ctx->c;
	result[9] = ctx->c >> 8;
	result[10] = ctx->c >> 16;
	result[11] = ctx->c >> 24;
	result[12] = ctx->d;
	result[13] = ctx->d >> 8;
	result[14] = ctx->d >> 16;
	result[15] = ctx->d >> 24;
	memset(ctx, 0, sizeof(*ctx));
}

CMd5::CMd5(bool resultUpperCase)
{
	m_UpperCase = resultUpperCase;
}

CMd5::~CMd5()
{

}

char CMd5::hex2asc(unsigned char hex) 
{
	hex = hex & 0xF;
	return hex < 10 ? ('0' + hex) : (hex - 10 + 'a');
}

int CMd5::md5File(const char *filename, string *result)
{
	FILE *fp;
	int ret;

	if (filename == NULL || result == NULL) {
		return -1;
	}

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		return -2;
	}

	ret = md5File(fp, result);
	fclose(fp);

	return ret;
}

int CMd5::md5File(const char *filename, unsigned char *result)
{
	FILE *fp;
	int ret;

	if (filename == NULL || result == NULL) {
		return -1;
	}

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		return -2;
	}

	ret = md5File(fp, result);
	fclose(fp);

	return ret;
}

int CMd5::md5File(FILE *fp, string *result)
{
	unsigned char out[16];

	if (fp == NULL || result == NULL) {
		return -1;
	}

	if (md5File(fp, out) != 0) {
		return -2;
	}

	if (md5_to_string(out, result) != 0) {
		return -3;
	}

	return 0;
}

int CMd5::md5File(FILE *fp, unsigned char *result)
{
	char buff[BUFSIZ];
	unsigned char out[16];
	size_t len = 0;
	md5_t context;

	if (fp == NULL || result == NULL) {
		return -1;
	}

	md5Init(&context);

	while ((len = fread(buff ,sizeof(char), BUFSIZ, fp)) > 0) {
		md5Update(&context, buff, len);
	}
	md5Final(out, &context);
	memcpy(result, out, 16);
	return 0;
}

int CMd5::md5(const void *data, size_t len, string *result) 
{
	unsigned char out[16];

	if (data == NULL || result == NULL) {
		return -1;
	}

	if (md5(data, len, out) != 0) {
		return -2;
	}

	if (md5_to_string(out, result) != 0) {
		return -3;
	}

	return 0;
}

int CMd5::md5(const string &data, string *result)
{
	return md5(data.c_str(), data.length(), result);
}

int CMd5::md5(const void *data, size_t len, unsigned char *result)
{
	unsigned char out[16];
	md5_t context;

	if (data == NULL || result == NULL) {
		return -1;
	}

	md5Init(&context);
	md5Update(&context, data, len);
	md5Final(out, &context);
	memcpy(result, out, 16);

	return 0;
}

int CMd5::md5(const string &data, unsigned char *result)
{
	return md5(data.c_str(), data.length(), result);
}

int CMd5::md5_to_string(unsigned char *md5, string *result)
{
	if (md5 == NULL || result == NULL) {
		return -1;
	}

	result->clear();
	for (size_t i = 0; i < 16; ++ i) {
		result->push_back(hex2asc(md5[i] >> 4));
		result->push_back(hex2asc(md5[i]));
	}

	if (m_UpperCase) {
		for (int i = 0; i < (int)result->size(); i++) {
			int n = (*result)[i];
			(*result)[i] = toupper(n);
		}
	}

	return 0;
}

void CMd5::setResultUpperCase(bool resultUpperCase)
{
	m_UpperCase = resultUpperCase;
}
	
} //namespace cbl 

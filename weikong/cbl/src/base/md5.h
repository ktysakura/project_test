#if !defined(MD5_H__20181205)
#define MD5_H__20181205

#include <string>

using namespace std;

namespace cbl {
	
typedef struct {
	unsigned int lo, hi;
	unsigned int a, b, c, d;
	unsigned char buffer[64];
	unsigned int block[16];
} md5_t;

class CMd5 {
public:
	//resultUpperCase：结果是否表示成大写
	CMd5(bool resultUpperCase = false);
	virtual ~CMd5();

	int md5(const string &data, string *result);
	int md5(const string &data, unsigned char *result);

	int md5(const void *data, size_t len, string *result);
	int md5(const void *data, size_t len, unsigned char *result);

	int md5File(const char *filename, string *result);
	int md5File(const char *filename, unsigned char *result);

	int md5File(FILE *fp, string *reslut);
	int md5File(FILE *fp, unsigned char *result);

	void setResultUpperCase(bool resultUpperCase);

private:
	const void* md5Body(md5_t *ctx, const void *data, unsigned long size);
	void md5Init(md5_t *ctx);
	void md5Update(md5_t *ctx, const void *data, unsigned long size);
	void md5Final(unsigned char *result, md5_t *ctx);
	char hex2asc(unsigned char hex);
	int md5_to_string(unsigned char *md5, string *result);

private:
	bool m_UpperCase;
};
 	
} 

#endif
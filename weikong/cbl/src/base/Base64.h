#if !defined(BASE64_H__20190111)
#define BASE64_H__20190111

//在线base64和base64url验证网址：http://www.base64url.com/

#include <string>

using std::string;

namespace cbl {

class CBase64
{
public:
	//构造函数，指定是否采用url模式
	CBase64(bool urlMode = false);

	virtual ~CBase64();

	//常规的base64编码函数，成功返回目标结果字节数，失败返回负数
	int encode(const void *source, size_t slen, char *target, size_t tlen);

	//常规的base64编码函数，成功返回目标结果字节数，失败返回负数
	int encode(const void *source, size_t slen, string &target);
	
	//base64解码函数, 成功返回目标结果字节数，失败返回负数
	int decode(const char *source, size_t slen, string &target);

	//base64解码函数, 成功返回目标结果字节数，失败返回负数
	int decode(const char *source, size_t slen, void *target, size_t tlen);

private:
	size_t base64_encode_table(char *target, const void *source, size_t bytes, const char *table);

private:
	bool m_urlMode;
};
	
}

#endif

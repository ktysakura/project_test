#if !defined(BASE64_H__20190111)
#define BASE64_H__20190111

//����base64��base64url��֤��ַ��http://www.base64url.com/

#include <string>

using std::string;

namespace cbl {

class CBase64
{
public:
	//���캯����ָ���Ƿ����urlģʽ
	CBase64(bool urlMode = false);

	virtual ~CBase64();

	//�����base64���뺯�����ɹ�����Ŀ�����ֽ�����ʧ�ܷ��ظ���
	int encode(const void *source, size_t slen, char *target, size_t tlen);

	//�����base64���뺯�����ɹ�����Ŀ�����ֽ�����ʧ�ܷ��ظ���
	int encode(const void *source, size_t slen, string &target);
	
	//base64���뺯��, �ɹ�����Ŀ�����ֽ�����ʧ�ܷ��ظ���
	int decode(const char *source, size_t slen, string &target);

	//base64���뺯��, �ɹ�����Ŀ�����ֽ�����ʧ�ܷ��ظ���
	int decode(const char *source, size_t slen, void *target, size_t tlen);

private:
	size_t base64_encode_table(char *target, const void *source, size_t bytes, const char *table);

private:
	bool m_urlMode;
};
	
}

#endif

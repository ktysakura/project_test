#if !defined(CONSOLE_H__20181130)
#define CONSOLE_H__20181130

namespace cbl {
	
class CConsole 
{
public:
	//���캯���������ý��̷���һ���µĿ���̨����
	CConsole();

	//�����������ͷſ���̨����
	virtual ~CConsole();

private:
	int alloc();
	void free();
};

} 

#endif

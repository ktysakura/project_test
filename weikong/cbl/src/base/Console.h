#if !defined(CONSOLE_H__20181130)
#define CONSOLE_H__20181130

namespace cbl {
	
class CConsole 
{
public:
	//构造函数，给调用进程分配一个新的控制台窗口
	CConsole();

	//析构函数，释放控制台窗口
	virtual ~CConsole();

private:
	int alloc();
	void free();
};

} 

#endif

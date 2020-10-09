#include "shell.h"
#if defined(_WIN32)  || defined(WIN32)
#include <Windows.h>
#include "str.h"
#elif defined(__linux__)
#endif

namespace cbl{
static bool   IsOsVersionVistaOrGreater();
int execShell(const string& sExeCmd, bool bShow /*= false*/)
{
	return execShell(sExeCmd, "", bShow);
}

int execShell(const string& sExeCmd,const string&  sParam, bool bShow/*= false*/)
{

#if defined(_WIN32)  || defined(WIN32)
	HANDLE hProcess;
	if ( IsOsVersionVistaOrGreater()){
		SHELLEXECUTEINFOA shExecInfo;

		ZeroMemory(&shExecInfo, sizeof(SHELLEXECUTEINFO));
		shExecInfo.cbSize = sizeof(shExecInfo);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = "open";
		shExecInfo.lpFile = sExeCmd.c_str();
		shExecInfo.lpParameters = sParam.c_str();
		shExecInfo.lpDirectory = NULL;
		shExecInfo.nShow = bShow ? SW_SHOW : SW_HIDE;
		shExecInfo.hInstApp = NULL;
		if (!ShellExecuteExA(&shExecInfo)){
			DWORD dwStatus=GetLastError();
			if(dwStatus==ERROR_CANCELLED){
				printf("提升权限被用户拒绝\n");
			}
			else if(dwStatus==ERROR_FILE_NOT_FOUND){
				printf("所要执行的文件没有找到\n");
			}
			return -1;
		}
		hProcess = shExecInfo.hProcess;
	}else{
		HINSTANCE pid;
		pid = ShellExecuteA(NULL, "open", replaceall(sExeCmd, "\\", "\\\\").c_str(), sParam.c_str(), NULL, SW_SHOWNORMAL);
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, (DWORD)pid);
	}

	if (WaitForSingleObject(hProcess, INFINITE) != WAIT_OBJECT_0){
		return -2;
	}

	if (!CloseHandle(hProcess)){
		return -3;
	}

	return 0;

#elif defined(__linux__)

#endif

}


bool   IsOsVersionVistaOrGreater()
{
	OSVERSIONINFOEXA    ovex;
	char  szVersionInfo[1024] = { 0 };
	*szVersionInfo = '\x00';
	//设置参数的大小，调用并判断是否成功
	ovex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	if( !GetVersionExA(  (LPOSVERSIONINFOA) (&ovex)  )  )
	{
		printf("检查系统版本失败\n");
		return false;
	}
	//通过版本号，判断是否是vista及之后版本
	if(ovex.dwMajorVersion > 5)
	{
		return true; 
	}
	else
	{
		return false;
	}
}


}

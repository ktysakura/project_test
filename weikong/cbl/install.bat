@rem 先删除原来的内容
@echo off

@rem 记录当前位置
set CURDIR=%cd%

@rem 判断middleware目录是否存在
if not defined MIDDLEWARE (
	echo 没有middleware目录
	@pause
	exit
) else (
	@rem 删除include下的cbl目录
	@rmdir /s /q %MIDDLEWARE%\include\cbl 2> nul
	@md %MIDDLEWARE%\include\cbl
	
	@rem 删除lib下的库文件
	@del /q %MIDDLEWARE%\lib\Debug\cbl.lib 2> nul
	@del /q %MIDDLEWARE%\lib\Release\cbl.lib 2> nul
)

@rem 拷贝src目录下的头文件
for /f "delims=\" %%f in ('dir /b /a-d /o-d "%CURDIR%\src\*.h"') do (
	@xcopy %CURDIR%\src\%%f %MIDDLEWARE%\include\cbl\ /s /y /f
)

@rem 拷贝src目录下的子目录里的头文件
for /f %%i in ('dir /b /ad "%CURDIR%\src"') do (
	@md %MIDDLEWARE%\include\cbl\%%i
	
	for /f "delims=\" %%f in ('dir /b /a-d /o-d "%CURDIR%\src\%%i\*.h"') do (
		@xcopy %CURDIR%\src\%%i\%%f %MIDDLEWARE%\include\cbl\%%i /s /y /f
	)
)
	
::@rem 拷贝库文件
@xcopy %CURDIR%\lib\Debug\cbl.lib %MIDDLEWARE%\lib\Debug\ /s /y /f
@xcopy %CURDIR%\lib\Release\cbl.lib %MIDDLEWARE%\lib\Release\ /s /y /f

@pause
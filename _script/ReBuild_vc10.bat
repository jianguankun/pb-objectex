@COLOR 09
@ECHO ����VS2010��װĿ¼
@SET VCINSTALLDIR=%VS100COMNTOOLS%/../IDE/
@if "%VCINSTALLDIR%"=="" (ECHO ����VS2005��װ·��ʧ�� goto end) ELSE (ECHO ����VS2010��װ·���ɹ�)
@SET PATH=%PATH%;%VCINSTALLDIR%

@ECHO ���� pbex
@ECHO ���� pbex:Debug
@devenv.exe "..\Builds\vc10\pbex.sln" /reBuild "Debug"
@ECHO ���� pbex:Release
@devenv.exe "..\Builds\vc10\pbex.sln" /reBuild "Release"
@ECHO ���� pbex ���

:end
@ECHO %DATE% %TIME%
@ECHO ����

PAUSE
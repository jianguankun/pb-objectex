@COLOR 09
@ECHO 搜索VS2010安装目录
@SET VCINSTALLDIR=%VS100COMNTOOLS%/../IDE/
@if "%VCINSTALLDIR%"=="" (ECHO 搜索VS2005安装路径失败 goto end) ELSE (ECHO 搜索VS2010安装路径成功)
@SET PATH=%PATH%;%VCINSTALLDIR%

@ECHO 编译 pbex
@ECHO 编译 pbex:Debug
@devenv.exe "..\Builds\vc10\pbex.sln" /reBuild "Debug"
@ECHO 编译 pbex:Release
@devenv.exe "..\Builds\vc10\pbex.sln" /reBuild "Release"
@ECHO 编译 pbex 完成

:end
@ECHO %DATE% %TIME%
@ECHO 结束

PAUSE
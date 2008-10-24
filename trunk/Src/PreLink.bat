@echo off

REM $Id$

set info=echo *

REM _ACP_ATLPROV was introduced in VC7. If not set, assume VC6.
REM _MSDEV_BLD_ENV_ was removed in VC8. Don't care about it if _ACP_ATLPROV is set.

if "%_ACP_ATLPROV%" == "" goto _MSDEV_BLD_ENV_(%_MSDEV_BLD_ENV_%)
set msdev=rem
set devenv=devenv
goto Configure

:_MSDEV_BLD_ENV_(1)
set msdev=msdev
set devenv=rem
goto Configure

:_MSDEV_BLD_ENV_()
echo Merge.dsp/Merge.vcproj pre-link script
echo Not intended for direct invocation through user interface
echo Pre-link command line: PreLink.bat $(IntDir) $(TargetPath)
pause
exit

:Configure
%info% Configure

cd
REM Enable echo lines below if you need to debug this script
REM echo %0
REM echo $(IntDir) = %1
REM echo $(TargetPath) = %2
del %2

REM Create build directories
mkdir ..\Build\expat
mkdir ..\Build\pcre
mkdir ..\Build\heksedit
goto %1

:.\..\BuildTmp\MergeDebug
%info% ANSI/Debug
mkdir ..\Build\MergeDebug
goto Debug

:.\..\BuildTmp\MergeUnicodeDebug
%info% UNICODE/Debug
mkdir ..\Build\MergeUnicodeDebug
goto Debug

:.\..\BuildTmp\MergeRelease
%info% ANSI/Release
mkdir ..\Build\MergeRelease
goto Release

:.\..\BuildTmp\MergeUnicodeRelease
%info% UNICODE/Release
mkdir ..\Build\MergeUnicodeRelease
goto Release

:Debug
%info% Debug

REM Build expat
cd %2\..\..\..\Externals\expat\lib
%msdev% "expat.dsp" /make "expat - Win32 Debug"
%devenv% "expat.vcproj" /build "Debug"
cd %2\..\..\expat
copy lib\debug\libexpat.dll %2\..\

REM Build SCEW
cd %2\..\..\..\Externals\scew\win32
%msdev% "scew.dsp" /make "scew - Win32 Debug"
%devenv% "scew.vcproj" /build "Debug"

REM Build PCRE
cd %2\..\..\..\Externals\pcre\Win32
%msdev% "pcre.dsp" /make "pcre - Win32 Debug"
%devenv% "pcre.vcproj" /build "Debug"
cd %2\..\..\pcre
copy pcre.dll %2\..\

REM Build heksedit
cd %2\..\..\..\Externals\heksedit
%msdev% "heksedit.dsp" /make "heksedit - Win32 Debug"
%devenv% "heksedit.vcproj" /build "Debug"
cd %2\..\..\heksedit\Debug
copy heksedit.dll %2\..\
mkdir %2\..\heksedit.lng
copy heksedit.lng\*.* %2\..\heksedit.lng

goto Common

:Release
%info% Release

REM Build expat
cd %2\..\..\..\Externals\expat\lib
%msdev% "expat.dsp" /make "expat - Win32 Release"
%devenv% "expat.vcproj" /build "Release"
cd %2\..\..\expat
copy lib\release\libexpat.dll %2\..\


REM Build SCEW
cd %2\..\..\..\Externals\scew\win32
%msdev% "scew.dsp" /make "scew - Win32 Release"
%devenv% "scew.vcproj" /build "Release"

REM Build PCRE
cd %2\..\..\..\Externals\pcre\Win32
%msdev% "pcre.dsp" /make "pcre - Win32 Release"
%devenv% "pcre.vcproj" /build "MinSizeRel"
cd %2\..\..\pcre
copy pcre.dll %2\..\

REM Build heksedit
cd %2\..\..\..\Externals\heksedit
%msdev% "heksedit.dsp" /make "heksedit - Win32 Release"
%devenv% "heksedit.vcproj" /build "Release"
cd %2\..\..\heksedit\Release
copy heksedit.dll %2\..\
mkdir %2\..\heksedit.lng
copy heksedit.lng\*.* %2\..\heksedit.lng

goto Common

:Common
%info% Common
@echo off
setlocal

if "%Platform%" neq "x64" (
    echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt or alternatively call vcvarsall.
    exit /b 1
)

set "root_dir=%cd%"
set "build_dir=%root_dir%\build"
set "src_dir=%root_dir%\src"

set "common_comp_options=/Oi /W4 /wd4100"
set "common_link_options=/INCREMENTAL:no /opt:icf /opt:ref"

if "%1"=="debug" (
	set "comp_options=%common_comp_options% /DSC_DEBUG=1 /Od /Zo /Z7 /MTd /RTC1"
	set "link_options=%common_link_options%"
) else if "%1"=="release" (
	set "comp_options=%common_comp_options% /O2"
	set "link_options=%common_link_options%"
) else (
	echo Illegal first argument^, must be one of ^<debug^|release^>
	goto end
)

if "%2" neq "" (
	echo Illegal number of arguments^, expected^: build ^<debug^|release^>
	goto end
)

if not exist %build_dir% mkdir %build_dir%
cd %build_dir%

cl /nologo /diagnostics:caret %comp_options% %src_dir%\main.c /link %link_options% /fixed /SUBSYSTEM:console /out:smolc.exe /pdb:smolc.pdb

:end
endlocal

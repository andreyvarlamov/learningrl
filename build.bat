@ECHO OFF

SETLOCAL

set CommonLibDir=C:\dev\shared\libs
set CommonIncludeDir=C:\dev\shared\include

set RootDir=%CurrProjDir:~0,-1%

set BuildDir=%RootDir%\build
set SourceDir=%RootDir%\source

set CompilerOptions=/I%CommonIncludeDir% /MTd /nologo /FC /GR- /Z7 /EHa- /Od /Oi
set CompilerWarningOptions=/WX /W4 /wd4201 /wd4100 /wd4189 /wd4505
set LinkOptions=/LIBPATH:%CommonLibDir% /INCREMENTAL:NO /OPT:REF /SUBSYSTEM:CONSOLE
set LinkLibs=raylibdll.lib raylib.lib

pushd %BuildDir%

cl %SourceDir%\learningrl.cpp %CompilerOptions% %CompilerWarningOptions% /link %LinkOptions% %LinkLibs%

popd

ENDLOCAL

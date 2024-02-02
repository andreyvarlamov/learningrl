@ECHO OFF

SETLOCAL

set RootDir=%CurrProjDir:~0,-1%

devenv %RootDir%\build\vs_debug\learningrl.sln

ENDLOCAL

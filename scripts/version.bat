@echo off
set VERSION_FILE=src\version.h
cat %VERSION_FILE% | grep QNAPI_DISPLAYABLE_VERSION > tmp.txt
set /p VERSION_LINE=<tmp.txt
rem tmp.txt
echo %VERSION_LINE:~35,-1%


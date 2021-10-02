@echo off
%HOMEDRIVE%
cd %HOMEPATH%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
start "" "C:\Program Files\Git\git-bash.exe"


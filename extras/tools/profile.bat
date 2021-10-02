@echo off

SET CURDIR=%~dp0

NET SESSION
IF %ERRORLEVEL% NEQ 0 GOTO ELEVATE
GOTO ADMINTASKS

:: Asking for admin rights
:ELEVATE
CD %CURDIR%
MSHTA "javascript: var shell = new ActiveXObject('shell.application'); shell.ShellExecute('%~nx0', '', '', 'runas', 1);close();"
EXIT


:: Running as administrator
:ADMINTASKS
xperf -on Latency ^
-stackwalk Profile ^
-minbuffers 16 -maxbuffers 1024 -flushtimer 0 ^
-f C:\tmp.etl

START /WAIT %CURDIR%\..\..\_out\bin\game.exe

xperf -d %CURDIR%\..\..\profile.etl


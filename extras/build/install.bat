@echo off

:: Config
set GAME_EXE=game.exe
set GAME_DLL=gdll.dll
set CRASH_EXE=gerr.exe

set GAME_EXE_PDB=game.pdb
set GAME_DLL_PDB=gdll.pdb

:: Variables
for %%i in ("%~dp0.") do SET "BATPATH=%%~fi"
set BINDIR=%BATPATH%\..\bin\win64
set WIN32=C:\Windows\System32


:: Bail out, if we have do not have two arguments.
set argC=0
for %%x in (%*) do Set /A argC+=1
if NOT "%argC%"=="2" goto help

:: Copy over all executable files.
mkdir "%2\bin" 2> nul
%WIN32%\xcopy.exe /Q /Y /R /I "%1\bin\%GAME_EXE%" "%2\bin" > nul || goto :error
%WIN32%\xcopy.exe /Q /Y /R /I "%1\bin\%GAME_DLL%" "%2\bin" > nul || goto :error
%WIN32%\xcopy.exe /Q /Y /R /I "%1\bin\%CRASH_EXE%" "%2\bin" > nul || goto :error

:: Copy debug files.
%WIN32%\xcopy.exe /Q /Y /R /I "%1\bin\%GAME_EXE_PDB%" "%2\bin" > nul || goto :error
%WIN32%\xcopy.exe /Q /Y /R /I "%1\bin\%GAME_DLL_PDB%" "%2\bin" > nul || goto :error

:: Copy over all data files.
mkdir "%2\data" 2> nul
for %%f in ("%1\data\*.idx" "%1\data\*.dat" "%1\data\*.xdi") do (
    %WIN32%\xcopy.exe /Q /Y /R /I /J "%%f" "%2\data" > nul || goto :error
)

:: Apply ducible.
%BINDIR%\ducible.exe "%2\bin\%GAME_EXE%" "%2\bin\%GAME_EXE_PDB%" > nul || goto :error
%BINDIR%\ducible.exe "%2\bin\%GAME_DLL%" "%2\bin\%GAME_DLL_PDB%" > nul || goto :error
%BINDIR%\ducible.exe "%2\bin\%CRASH_EXE%" > nul || goto :error

:: Print checksums.
for %%f in ("%2\bin\*.exe" "%2\bin\*.dll") do (
    %BINDIR%\sha256deep.exe "%%f"
)
echo.
for %%f in ("%2\data\*") do (
    %BINDIR%\sha256deep.exe "%%f"
)

goto out

:help
echo Installs the required files from SRC into DST.
echo.
echo usage: install.bat SRC DST
echo.
echo SRC    Source directory
echo DST    Destination directory

:out
exit /b 0

:error
echo error: installing "%1" to "%2"
exit /b 1

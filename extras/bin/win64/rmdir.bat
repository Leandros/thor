@echo OFF

@echo cleaning...

set argC=0
for %%x in (%*) do Set /A argC+=1
if "%argC%"=="0" goto help

:: Deletion
:loop
rd /s /q "%1"
@echo removed %1

shift
if not "%~1"=="" goto loop
goto end

:help
@echo usage: rmdir.bat DIRS...

:end
@echo success

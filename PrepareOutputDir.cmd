@echo off
setlocal
rem Preserve the build entry point while sharing the corrected PowerShell logic.
rem The final dot prevents a trailing backslash from escaping the closing quote.
if "%~1"=="" goto Usage
if "%~2"=="" goto Usage
if "%~3"=="" goto Usage
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0PrepareOutputDir.ps1" "%~1\." "%~2" "%~3"
exit /b %errorlevel%

:Usage
echo Usage: %~nx0 SolutionDirectory ConfigurationName Architecture
exit /b 1

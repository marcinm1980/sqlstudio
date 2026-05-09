@echo off
setlocal enableextensions

rem -------------------------------------------------------------------------------
rem Check parameter
if [%1] == [] goto Usage
if [%2] == [] goto Usage
if [%3] == [] goto Usage

set "ESC="
for /f "delims=" %%e in ('echo prompt $E^| cmd') do set "ESC=%%e"

set "C_INFO=%ESC%[96m"
set "C_OK=%ESC%[92m"
set "C_WARN=%ESC%[93m"
set "C_ERR=%ESC%[91m"
set "C_RESET=%ESC%[0m"

call :Info "Preparing test output directory..."

rem -------------------------------------------------------------------------------
rem Set directory variables
set SOLUTION_DIR=%~1
set CONFIGURATION=%~2
set ARCH=%~3

set TEST_ROOT=%~dp0
if "%TEST_ROOT:~-1%"=="\" set TEST_ROOT=%TEST_ROOT:~0,-1%

set TARGET_DIR=%SOLUTION_DIR%bin\%ARCH%\%CONFIGURATION%
set TEST_DATA_DIR=%TEST_ROOT%\data

call :Info "Solution directory: %SOLUTION_DIR%"
call :Info "Test root directory: %TEST_ROOT%"
call :Info "Configuration: %CONFIGURATION%"
call :Info "Architecture: %ARCH%"
call :Info "Target directory: %TARGET_DIR%"

if not exist "%TARGET_DIR%" (
  call :Warn "Target directory does not exist yet, creating it..."
  mkdir "%TARGET_DIR%" 1>nul 2>nul
)

if not exist "%TEST_DATA_DIR%" (
  call :Warn "No local test data directory found at %TEST_DATA_DIR%"
  goto Done
)

call :Info "Copy test data files ..."
if not exist "%TARGET_DIR%\data" mkdir "%TARGET_DIR%\data" 1>nul 2>nul
xcopy /i /s /y /d "%TEST_DATA_DIR%\*" "%TARGET_DIR%\data\." 1> nul 2> nul
if errorlevel 1 (
  call :Warn "Some copy operations returned non-zero status (files may still be up to date)."
)

:Done
call :Ok "Test output directory preparation complete."
endlocal & exit /b 0

:Info
echo %C_INFO%[INFO]%C_RESET% %~1
exit /b 0

:Ok
echo %C_OK%[ OK ]%C_RESET% %~1
exit /b 0

:Warn
echo %C_WARN%[WARN]%C_RESET% %~1
exit /b 0

:Err
echo %C_ERR%[ERR ]%C_RESET% %~1
exit /b 0

:Usage
call :Err "Usage: %0 SolutionDirectory ConfigurationName Architecture"
call :Err "Example: %0 \"D:\develop\MySQL\MySQLStudio\\\" Debug x64"
endlocal & exit /b 1

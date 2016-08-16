@echo off & setlocal enableextensions enabledelayedexpansion

set _PK_GEN="Visual Studio 9 2008"

call "%~d0%~p0\scripts\generate-build-common.cmd" %*
if %errorlevel% == 2 exit /b 1

if not defined _MASTER_BUILD pause

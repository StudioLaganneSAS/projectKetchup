@echo off & setlocal enableextensions enabledelayedexpansion

set _PK_GEN="Visual Studio 10 2010"

call "%~d0%~p0\scripts\generate-build-common.cmd" %*
if %errorlevel% == 2 exit /b 1

if not defined _MASTER_BUILD pause

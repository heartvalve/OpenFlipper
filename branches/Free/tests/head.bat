@echo off
setlocal enabledelayedexpansion
if [%1] == [] goto usage
if [%2] == [] goto usage

SET /a counter=0

for /f "usebackq delims=" %%a in (%2) do (
if "!counter!"=="%1" goto exit
echo %%a
set /a counter+=1
)

goto exit

:usage
echo Usage: head.bat COUNT FILENAME

:exit

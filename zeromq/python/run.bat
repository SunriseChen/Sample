@echo off

if '%1'=='' goto Usage
if '%2'=='' goto Usage

start "Node1" python node.py %1
start "Node2" python node.py %2
goto End

:Usage
echo Usage:
rem Request-Reply
echo %0 req rep
rem Publish-Subscribe
echo %0 pub sub
rem Parallel Pipeline
echo %0 push pull

echo %0 req router
echo %0 dealer rep
echo %0 dealer router
echo %0 dealer dealer
echo %0 router router
echo %0 pair pair
goto End

:End


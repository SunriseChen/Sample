@echo off

if '%1'=='rr' goto RequestReply
if '%1'=='ps' goto PublishSubscribe
if '%1'=='pp' goto ParallelPipeline
goto Usage

:RequestReply
start "Server" python node.py rep
start "Client1" python node.py req
start "Client2" python node.py req
start "Client3" python node.py req
goto End

:PublishSubscribe
start "Publisher" python node.py pub
start "Subscriber1" python node.py sub
start "Subscriber2" python node.py sub
start "Subscriber3" python node.py sub
goto End

:ParallelPipeline
start "Ventilator" python node.py ventilator
start "Worker1" python node.py worker
start "Worker2" python node.py worker
start "Worker3" python node.py worker
start "Sink" python node.py sink
goto End

:Usage
echo Usage:
rem Request-Reply
echo %0 rr
rem Publish-Subscribe
echo %0 ps
rem Parallel Pipeline
echo %0 pp

echo %0 req router
echo %0 dealer rep
echo %0 dealer router
echo %0 dealer dealer
echo %0 router router
echo %0 pair pair
goto End

:End


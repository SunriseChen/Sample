@echo off

if '%1'=='rr' goto RequestReply
if '%1'=='ps' goto PublishSubscribe
if '%1'=='pp' goto ParallelPipeline
goto Usage

:RequestReply
start "Server" python node.py rep %2
start "Client1" python node.py req %2
start "Client2" python node.py req %2
start "Client3" python node.py req %2
goto End

:PublishSubscribe
start "Publisher" python node.py pub %2
start "Subscriber1" python node.py sub %2
start "Subscriber2" python node.py sub %2
start "Subscriber3" python node.py sub %2
goto End

:ParallelPipeline
start "Ventilator" python node.py ventilator %2
start "Worker1" python node.py worker %2
start "Worker2" python node.py worker %2
start "Worker3" python node.py worker %2
start "Sink" python node.py sink %2
goto End

:Usage
echo Usage:
rem Request-Reply
echo %0 rr
echo %0 rr thread_count
rem Publish-Subscribe
echo %0 ps
echo %0 ps thread_count
rem Parallel Pipeline
echo %0 pp
echo %0 pp thread_count
goto End

:End


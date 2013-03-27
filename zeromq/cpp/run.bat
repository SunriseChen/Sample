@echo off

call scons -Q
if '%1'=='rr' goto RequestReply
if '%1'=='ps' goto PublishSubscribe
if '%1'=='pp' goto ParallelPipeline
goto Usage

:RequestReply
start "Server" node rep %2
start "Client1" node req %2
start "Client2" node req %2
start "Client3" node req %2
goto End

:PublishSubscribe
start "Publisher" node pub %2
start "Subscriber1" node sub %2
start "Subscriber2" node sub %2
start "Subscriber3" node sub %2
goto End

:ParallelPipeline
start "Ventilator" node ventilator %2
start "Worker1" node worker %2
start "Worker2" node worker %2
start "Worker3" node worker %2
start "Sink" node sink %2
goto End

:Usage
echo Usage:
echo		Request-Reply:
echo		%0 rr [thread_count]
echo.
echo		Publish-Subscribe:
echo		%0 ps [thread_count]
echo.
echo		Parallel Pipeline:
echo		%0 pp [thread_count]
goto End

:End


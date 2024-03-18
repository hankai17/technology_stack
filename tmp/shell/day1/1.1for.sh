#!/bin/bash

PIDS=`ps aux | grep nginx | grep "worker process is shutting down" |awk '{print $2}'`

for PID in $PIDS ;
do
	echo $PID
	#  kill $PID > /dev/null 2>&1;
done

#ps aux --forest | grep nginx

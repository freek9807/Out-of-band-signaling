#!/bin/env bash

./supervisor.o 8 > supervisor.log 2> supervisor-err.log &
PID=$!

sleep 2s

for i in 1 2 3 4 5 6 7 8 9 10 ; do 
	./client.o 5 8 20 > client-$i.log &  
	INDEX=$((i+10))
	./client.o 5 8 20 > client-$INDEX.log &  
    sleep 1s
done 

for i in 1 2 3 4 5 6 ; do 
	kill -INT $PID 
	sleep 10s  
done 

pkill -INT supervisor.o
pkill -INT supervisor.o


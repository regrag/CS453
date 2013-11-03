#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib

for sleeptime in 0  
do
	for items in 10 100 1000
	do
		for p in  1 2 3 4 5 
		do
			for c in 1 2 3 4 5
			do
				./pc 10 $items $p $c $sleeptime > /dev/null
			done
		done
	done
done

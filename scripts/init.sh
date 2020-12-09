#!/bin/bash
SRCFILES=( "sequential.c" "parallel.c" "sequential_percentage.c" "parallel_percentage.c" )

if [ -n "`which mpiexec`" ]
then
	sudo apt-get install openmpi-common openmpi-bin libopenmpi-dev
	for DIRNAME in ( "src" "inputs" )
	if [ ! -e ../$DIRNAME ]
	then
		mkdir ../$DIRNAME
	fi
	touch ../src/${SRCFILES[@]}
	echo "Make sure you have you have 10x1M.txt, 10x2M.txt and 10x5M.txt in ../inputs before running test_time.sh. You can also change the variable INPUTFILES inside the script."
fi

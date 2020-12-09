#!/bin/bash
INPUTDIR="../inputs"
INPUTFILES=( "10x1M.txt" "10x2M.txt" "10x5M.txt" )
TVALUES=( 1 2 4 8 )
SRCDIR="../src"
TEMPDIR=".temp"
CODE=( "sequential.c" "parallel.c" "sequential_percentage.c" "parallel_percentage.c" )
SEQ=0
PAR=1
SEQP=2
PARP=3
OUTPUT="$1.csv"
CFLAGS="-fopenmp -Wextra -O3"
COMPILER="gcc"
NUM_EXECS=20

if [[ "$1" == "--help" ]]
then
	echo "EXECUTION: bash this_script.sh output_csv"
	echo "this will save the output to output_csv.csv"
	echo "REQUIREMENTS: ../inputs with the files $INPUTFILES"
	echo "../src with the files $CODE"
	echo "sequential.c and parallel.c both output the result+\n+time of execution"
	echo "the _percentage.c files output output the result+\n+percentage of time spent on sequential parts"
	exit
fi

echo "Creating $OUTPUT, temporary dir ($TEMPDIR) and compiling code"

echo "Source;Threads;Input;Result;Output" > $OUTPUT
mkdir $TEMPDIR
for SRCFILE in "${CODE[@]}"
do
	$COMPILER $SRCDIR/$SRCFILE -o $TEMPDIR/${SRCFILE%".c"}.out 
done

for VERSION in "${CODE[@]}"
do
	echo "Running ${VERSION%".c"} version"
	TITERS=$TVALUES
	if [[ !$VERSION == *"parallel"* ]]; then
		TITERS=( 1 )
	fi
	for (( ITER=0; ITER<${NUM_EXECS}; ITER++ ))
	do
		for THREADS in "${TITERS[@]}"
		do
			for INPUT in "${INPUTFILES[@]}"
			do
				./$TEMPDIR/${VERSION%".c"}.out < $INPUTDIR/$INPUT > $TEMPDIR/${VERSION%".c"}_${INPUT%".txt"}.txt
				RESULT="`tail -n 1 $TEMPDIR/${VERSION%".c"}_${INPUT%".txt"}.txt`" # is either a percentage or a time count
				OUTSTATUS="ok"
				if [ "`head --lines=-2 $TEMPDIR/${VERSION%".c"}_${INPUT%".txt"}.txt`" != "`head --lines=-2 $TEMPDIR/sequential_${INPUT%".txt"}.txt`" ]
				then
					OUTSTATUS="notok"
				fi
				echo "${VERSION%".c"};$THREADS;${INPUT%".txt"};$RESULT;$OUTSTATUS" >> $OUTPUT
			done
		done
	done
done
echo "Removing $TEMPDIR. All output files and executables are going to be deleted."
rm -rf $TEMPDIR

echo "Saved information to $OUTPUT"

CC=mpicc
CFLAGS=-O3 -Wextra
EXEC=mpiexec
EFLAGS=--oversubscribe
K=1
N=2

run: run_par run_seq

run_par: all
	${EXEC} -n 2 par.out ${EFLAGS} < a.in

run_seq: all
	${EXEC} -n 1 seq.out ${EFLAGS} < a.in

all: input seq par

input:
	python2 ../scripts/geninput.py ${K} ${N} > a.in

seq: sequential.c
	${CC} sequential.c ${CFLAGS} -o seq.out

par: parallel.c
	${CC} parallel.c ${CFLAGS} -o par.out

clean:
	rm *.out
	rm *.in

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define DIM 3

int intceil(int x, int y) {
	return (x+y-1)/y;
}

int main(int argc, char **argv) {
	int scanfArgs = 0;
	int i, j, k, n, c;
	double dmin, dx;
	double *x, *mean, *sum;
	int *cluster, *count, color;
	int flips;

	//MPI
	int my_rank, n_procs;
	int xs; //xsize
	int *sendcounts, *displs;
	double stime, etime, wstime;

	MPI_Init(&argc, &argv);
	stime = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
	sendcounts = (int *)malloc(sizeof(int)*n_procs);
	displs = (int *)malloc(sizeof(int)*n_procs);

	if (my_rank == 0) {
		scanfArgs += scanf("%d", &k);
		scanfArgs += scanf("%d", &n);
	}
	MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	xs = (my_rank == n_procs-1 ? n/n_procs : intceil(n, n_procs));
	if (!xs) {
		printf("closing %d", my_rank);
		MPI_Finalize();
	}

	//todo mundo tem o "mesmo": mean
	//cada um tem o seu, mas todos de mesmo tamanho: sum, count
	//cada um tem o seu, com tamanho reduzido: x, cluster
	if (my_rank == 0) {
		x = (double *)malloc(sizeof(double)*DIM*n);
	} else {
		x = (double *)malloc(sizeof(double)*DIM*xs);
	}
	mean = (double *)malloc(sizeof(double)*DIM*k);
	sum= (double *)malloc(sizeof(double)*DIM*k);
	cluster = (int *)malloc(sizeof(int)*xs);
	count = (int *)malloc(sizeof(int)*k);

	for (i = 0; i<xs; i++)
		cluster[i] = 0;

	if (my_rank == 0) {
		for (i = 0; i<k; i++)
			scanfArgs += scanf("%lf %lf %lf", mean+i*DIM, mean+i*DIM+1, mean+i*DIM+2);
		for (i = 0; i<n; i++)
			scanfArgs += scanf("%lf %lf %lf", x+i*DIM, x+i*DIM+1, x+i*DIM+2);
		if (scanfArgs < 3*(k+n)+2)
			exit(EXIT_FAILURE);
	}
	
	MPI_Bcast(mean, k*DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/*
	for (i =0 ; i< k; i++)
		printf("%d has mean[%d]=%lf\n", my_rank, i, mean[i]);
	*/

	for (i = 0; i < n_procs-1; i++) {
		sendcounts[i] = DIM*intceil(n, n_procs);
		displs[i] = DIM*i*intceil(n, n_procs);
	}
	sendcounts[n_procs-1] = DIM*(n/n_procs);
	displs[n_procs-1] = DIM*(n_procs-1)*intceil(n, n_procs);

	MPI_Scatterv(x, sendcounts, displs, MPI_DOUBLE, x, xs*DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/*
	if (my_rank == 0) {
		for (i =0; i< n*DIM; i++)
			printf("Global has x[%d]=%lf\n", i, x[i]);
	}
	for (i =0; i< xs*DIM; i++)
		printf("%d has x[%d]=%lf\n", my_rank, i, x[i]);
	*/

	wstime = MPI_Wtime();
	flips = n;
	while (flips>0) {
		flips = 0;
		for (j = 0; j < k; j++) {
			count[j] = 0;
			for (i = 0; i < DIM; i++)
				sum[j*DIM+i] = 0.0;
		}
		for (i = 0; i < xs; i++) {
			dmin = -1; color = cluster[i];
			for (c = 0; c < k; c++) {
				dx = 0.0;
				for (j = 0; j < DIM; j++)
					dx +=  (x[i*DIM+j] - mean[c*DIM+j])*(x[i*DIM+j] - mean[c*DIM+j]);
				if (dx < dmin || dmin == -1) {
					color = c;
					dmin = dx;
				}
			}
			if (cluster[i] != color) {
				flips++;
				cluster[i] = color;
			}
		}

		for (i = 0; i < xs; i++) {
			count[cluster[i]]++;
			for (j = 0; j < DIM; j++)
				sum[cluster[i]*DIM+j] += x[i*DIM+j];
		}

		//main recebe sum de todos
		if (my_rank == 0) {
			MPI_Reduce(MPI_IN_PLACE, sum, DIM*k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		} else {
			MPI_Reduce(sum, sum, DIM*k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		}
		//main recebe count de todos
		if (my_rank == 0) {
			MPI_Reduce(MPI_IN_PLACE, count, k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		} else {
			MPI_Reduce(count, count, k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		}
		//todos recebem flips de todos
		MPI_Allreduce(&flips, &flips, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		if (my_rank == 0) {
			for (i = 0; i < k; i++) {
				for (j = 0; j < DIM; j++) {
					mean[i*DIM+j] = sum[i*DIM+j]/count[i];
				}
			}
		}
		MPI_Bcast(mean, k*DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	etime = MPI_Wtime();
	if (my_rank == 0) {
		for (i = 0; i < k; i++) {
			for (j = 0; j < DIM; j++)
				printf("%5.2f ", mean[i*DIM+j]);
			printf("\n");
		}
		printf("\n%lf\n", (etime-wstime)/(etime-stime));
	}
	MPI_Finalize();
	#ifdef DEBUG
	for (i = 0; i < n; i++) {
		for (j = 0; j < DIM; j++)
			printf("%5.2f ", x[i*DIM+j]);
		printf("%d\n", cluster[i]);
	}
	#endif
	return(0);
}

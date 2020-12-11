#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define DIM 3
#define intceil(x, y) ((x+y-1)/y);

int main(int argc, char **argv) {
	int scanfArgs = 0;
	int i, j, k, n, c;
	double dmin, dx;
	double *x, *mean, *sum;
	int *cluster, *count, color;
	int flips;

	//variáveis relativas ao uso do MPI
	int my_rank, n_procs;
	unsigned int xs, clusters;
	int *int_buffer;
	double *double_buffer;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

	printf("Rank: %d, procs: %d\n", my_rank, n_procs);

	if (my_rank == 0) { //le k e n, cria buffer de int e buffer de double
		scanfArgs += scanf("%d", &k);
		scanfArgs += scanf("%d", &n);
		int_buffer = (int *)malloc(sizeof(int)*k);
		int_buffer[0] = k;
		int_buffer[1] = n;
		double_buffer = (double *)malloc(sizeof(int)*DIM*n);
	} else {
		int_buffer = (int *)malloc(sizeof(int)*2); //cria buffer temporario p/ ler k e n do Bcast
	}
	MPI_Bcast(int_buffer, 2, MPI_INT, 0, MPI_COMM_WORLD); //Bcast de k e n

	if (my_rank == 0) {
		xs = DIM*n;
		clusters = n;
	} else { //atribui k e n, cria os buffers de int e double
		k = int_buffer[0];
		n = int_buffer[1];

		free(int_buffer);
		int_buffer = (int *)malloc(sizeof(int)*k);
		double_buffer = (double *)malloc(sizeof(int)*DIM*n);

		xs = my_rank == n_procs-1 ? n/n_procs : intceil(n, n_procs);
		clusters = my_rank == n_procs-1 ? k/n_procs : intceil(k, n_procs);
	}

	x = (double *)malloc(sizeof(double)*xs); //cada um tem o seu, e n precisa de todo o espaço
	mean = (double *)malloc(sizeof(double)*DIM*k); //todo mundo tem
	sum= (double *)malloc(sizeof(double)*DIM*k); //cada um tem o seu, mas todos de mesmo tamanho
	cluster = (int *)malloc(sizeof(int)*clusters); //cada um tem o seu, e n precisa de todo o espaço
	count = (int *)malloc(sizeof(int)*k); //cada um tem o seu, mas todos de mesmo tamanho

	for (i = 0; i<n; i++)
		cluster[i] = 0;
	
	if (my_rank == 0) {
		for (i = 0; i<k; i++)
			scanfArgs += scanf("%lf %lf %lf", mean+i*DIM, mean+i*DIM+1, mean+i*DIM+2);
		for (i = 0; i<n; i++)
			scanfArgs += scanf("%lf %lf %lf", x+i*DIM, x+i*DIM+1, x+i*DIM+2);
		if (scanfArgs < 3*(k+n)+2) {
			exit(EXIT_FAILURE);
		}
	}
	MPI_Bcast(mean, k*DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Bcast de mean
	MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD); //TODO trocar isso por scatter; envia x pra todo mundo
	MPI_Finalize();
	return 0;


	flips = n;
	while (flips>0) {
		flips = 0;
		for (j = 0; j < k; j++) {
			count[j] = 0;
			for (i = 0; i < DIM; i++)
				sum[j*DIM+i] = 0.0;
		}
		for (i = 0; i < n; i++) {
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

		for (i = 0; i < n; i++) {
			count[cluster[i]]++;
			for (j = 0; j < DIM; j++)
				sum[cluster[i]*DIM+j] += x[i*DIM+j];
		}
		//aqui: todo mundo manda sum e count pra a main
		//			todo mundo manda flips pra todo mundo (reduz com or)
		for (i = 0; i < k; i++) {
			for (j = 0; j < DIM; j++) {
				mean[i*DIM+j] = sum[i*DIM+j]/count[i];
			}
		}
		//aqui: main manda mean pra todo mundo
	}
	//MPI_Finalize();
	for (i = 0; i < k; i++) {
		for (j = 0; j < DIM; j++)
			printf("%5.2f ", mean[i*DIM+j]);
		printf("\n");
	}
	#ifdef DEBUG
	for (i = 0; i < n; i++) {
		for (j = 0; j < DIM; j++)
			printf("%5.2f ", x[i*DIM+j]);
		printf("%d\n", cluster[i]);
	}
	#endif
	printf("\n2\n");
	return(0);
}

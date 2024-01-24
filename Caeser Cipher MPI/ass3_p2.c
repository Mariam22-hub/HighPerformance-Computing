#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main(int argc, char* argv[]) {
	
	static long num_steps = 1000000;
	double step;
	int i, my_rank, nprocess, chunkSize, start, end;
	double x, pi, partial_sum = 0.0, final_sum=0.0;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocess);

	//printf("before first if condition\n");
	printf("no of processes: %d\n", nprocess);
	
	chunkSize = num_steps / nprocess;
	start = my_rank * chunkSize;
	end = start + chunkSize;


	if (my_rank == 0) {
		printf("inside first if condition\n");

		step = 1.0 / (double)num_steps;

		printf("rank: %d\n", my_rank);

		printf("chunk size: %d\n", chunkSize);

	}


	MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	for (i = start; i < end; i++) {
		
		x = ((double)(i + 0.5)) * step;
		partial_sum += 4.0 / (1.0 + x * x);

	}

	//printf("reducing in rank: %d\n", my_rank);
	MPI_Reduce(&partial_sum, &final_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	
	if (my_rank == 0) {
		printf("rank: %d\n", my_rank);
		printf("calculating pi\n");
		
		pi = step * final_sum;
		printf("%.20f\n", pi);
	}

	//return 0;
	MPI_Finalize();

}
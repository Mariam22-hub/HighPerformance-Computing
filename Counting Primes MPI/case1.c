#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


int check_prime(int a)
{
	int c;

	if (a == 1) {
		return 0;
	}

	for (c = 2; c <= a - 1; c++)
	{
		if (a % c == 0)
			return 0;
	}
	return 1;
}


//int arr2[10000000];

int main(int argc, char* argv[]) {

	int rank, size_p;
	int i, j;

	// Creation of parallel processes
	MPI_Init(&argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size_p);

	double start, end;

	//MASTER
	if (rank == 0) {

		int x, y;

		printf("Please enter your lower and upper ranges: ");

		start = MPI_Wtime();

		scanf("%d %d", &x, &y);

		int count_master = 0;
		int elements_per_process = (y - x) / (size_p - 1);
		int index;

		int size = y - x;
		int arr[size + 10];

		int number = x;

		//store numbers in array
		for (i = 0; i < size; i++) {
			arr[i] = number;
			number++;
		}
		for (i = 0; i < size; i++) {
			printf("%d ", arr[i]);
		}

		if (size_p > 1) {

			//sending subarrays to slaves
			for (i = 0; i < size_p - 2; i++) {
				index = i * elements_per_process;

				MPI_Send(&elements_per_process, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
				MPI_Send(&arr[index], elements_per_process, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
			}

			index = i * elements_per_process;
			int elements_left = size - index;

			MPI_Send(&elements_left, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
			MPI_Send(&arr[index], elements_left, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
		}

		int countTmp;

		for (i = 1; i < size_p; i++) {
			MPI_Recv(&countTmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			count_master += countTmp;
		}
		printf("\n\nCount = %d\n", count_master);
		end = MPI_Wtime();

		printf("\n\nTime taken: %f seconds\n", end - start);

	}

	//SLAVES
	else {
		int count_slave = 0;
		int elements_receieved, size;

		MPI_Recv(&elements_receieved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int arr2[elements_receieved + 10];

		MPI_Recv(&arr2, elements_receieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);


		for (i = 0; i < elements_receieved; i++) {
			if (check_prime(arr2[i])) {
				count_slave++;
			}
		}

		//sending counts to slaves
		MPI_Send(&count_slave, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}

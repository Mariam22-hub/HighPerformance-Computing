#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<math.h>


int sz;
int *arr;
// Temporary array for slave process
int arr2[100000];

int main(int argc, char* argv[])
{
	double start, end;
	int pid, np, sum,
	elements_per_process,
	n_elements_recieved;
	double mean, mean2;
	MPI_Status status;

	// Creation of parallel processes
	MPI_Init(&argc, &argv);

	// find out process ID,
	// and how many processes were started
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	// master process
	if (pid == 0) {
	int i;
	 arr = (int *) malloc(sizeof(int)*sz);
    //  for(i=0;i<sz;i++)     //fill the array with random numbers
    //     arr[i]=rand()%100;
    printf("Hello from master process.\nNumber of slave processes is %d\n", np-1);
    printf("Please enter size of array...\n");
    scanf("%d", &sz);
	arr = (int*)malloc(sz * sizeof(int));
    printf("Please enter array elements ...\n");
    for(i=0; i<sz; i++){
    	scanf("%d", &arr[i]);
    }
	int index;
	start = MPI_Wtime();
		elements_per_process = sz / (np-1);
		if (np > 1) {
			for (i = 0; i < np - 2; i++) {                           
				index = i * elements_per_process;
				MPI_Send(&elements_per_process,
						1, MPI_INT, i+1, 0,
						MPI_COMM_WORLD);
    
				MPI_Send(&arr[index],
						elements_per_process,
						MPI_INT, i+1, 0,
						MPI_COMM_WORLD);
			}

			// last process compute maximum number from remaining elements
			index = i * elements_per_process; 
			int elements_left = sz - index;     
			MPI_Send(&elements_left,
					1, MPI_INT,
					i+1, 0,
					MPI_COMM_WORLD);
			MPI_Send(&arr[index],
					elements_left,
					MPI_INT, i+1, 0,
					MPI_COMM_WORLD);
		}

    	sum = 0;
	
		// collects partial max from other processes
		int tmpSum;
		for (i = 1; i < np; i++) {
			MPI_Recv(&tmpSum, 1, MPI_INT,
							i, 0,
							MPI_COMM_WORLD,
							&status);

			int sender = status.MPI_SOURCE;

			sum += tmpSum;
		}

    mean = (double) sum / (double)sz;
    printf("Master process announce the Mean is %f.\n", mean);
	if (np > 1) {
		for (i = 0; i < np - 2; i++) {                           
			index = i * elements_per_process;
			MPI_Send(&elements_per_process,
					1, MPI_INT, i+1, 0,
					MPI_COMM_WORLD);

			MPI_Send(&arr[index],
					elements_per_process,
					MPI_INT, i+1, 0,
					MPI_COMM_WORLD);
			
			MPI_Send(&mean,
					1, MPI_DOUBLE, i+1, 0,
					MPI_COMM_WORLD);
		}

		// last process compute maximum number from remaining elements
		index = i * elements_per_process; 
		int elements_left = sz - index;   
			
		MPI_Send(&elements_left,
				1, MPI_INT,
				i+1, 0,
				MPI_COMM_WORLD);
		MPI_Send(&arr[index],
				elements_left,
				MPI_INT, i+1, 0,
				MPI_COMM_WORLD);
		
		MPI_Send(&mean,
					1, MPI_DOUBLE, i+1, 0,
					MPI_COMM_WORLD);
	}
		
		double sumPow = 0.0;
		double tmpSum2;
		for (i = 1; i < np; i++) {
      		MPI_Recv(&tmpSum2, 1, MPI_DOUBLE,
					i, 0,
					MPI_COMM_WORLD,
					&status);
			int sender = status.MPI_SOURCE;

      		sumPow += tmpSum2;
		}
		double variance = sumPow / (double)sz;
		// double standardDeviation = sqrt(x);
    printf("Master process announce the Variance is %f.\n", variance);
	printf("Master process announce the Standard Deviation is %f.\n", sqrt(variance));
	printf("Thanks for using our program\n\n");
	
	}
	
	// slave processes
	else {
    
		MPI_Recv(&n_elements_recieved,
				1, MPI_INT, 0, 0,
				MPI_COMM_WORLD,
				&status);
		MPI_Recv(&arr2, n_elements_recieved,
				MPI_INT, 0, 0,
				MPI_COMM_WORLD,
				&status);

		int i;
    int partialSum = 0;
    for (i = 0; i < n_elements_recieved; i++){
		partialSum += arr2[i];
    }
    MPI_Send(&partialSum, 1, MPI_INT,
				0, 0, MPI_COMM_WORLD);

	MPI_Recv(&n_elements_recieved,
			1, MPI_INT, 0, 0,
			MPI_COMM_WORLD,
			&status);
	MPI_Recv(&arr2, n_elements_recieved,
			MPI_INT, 0, 0,
			MPI_COMM_WORLD,
			&status);
	MPI_Recv(&mean2,
			1, MPI_DOUBLE, 0, 0,
			MPI_COMM_WORLD,
			&status);
	
	double ps = 0.0 ;
	for(i=0; i< n_elements_recieved; i++){
		ps += (pow((arr2[i] - mean2), 2));
	}
		MPI_Send(&ps, 1, MPI_DOUBLE,
				0, 0, MPI_COMM_WORLD);
	}
	// cleans up all MPI state before exit of process
	end = MPI_Wtime();
	MPI_Finalize();
	if (pid == 0) { 
    printf("Runtime = %f\n", end-start);
}

	return 0;
}


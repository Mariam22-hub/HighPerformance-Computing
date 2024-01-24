#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// size of array
// #define n 10
int sz;
int *arr;
// Temporary array for slave process
int arr2[1000];

int main(int argc, char* argv[])
{
	//int a[maxLength];
	int pid, np,
	elements_per_process,
	n_elements_recieved;
  	char message[1000];
	// np -> no. of processes
	// pid -> process id
	
	MPI_Status status;

	// Creation of parallel processes
	MPI_Init(&argc, &argv);

	// find out process ID,
	// and how many processes were started
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	// master process
	if (pid == 0) {
    printf("Hello from master process.\nNumber of slave processes is %d\n", np-1);
    printf("Please enter size of array...\n");
    scanf("%d", &sz);
	arr = (int*)malloc(sz * sizeof(int));
    printf("Please enter array elements ...\n");
    
	int index, i;
    for(i=0; i<sz; i++){
    	scanf("%d", &arr[i]);
    }
		elements_per_process = sz / (np-1);
		// check if more than 1 processes are run
		if (np > 1) {
			// distributes the portion of array
			// to child processes
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

    int max = arr[0];

		// collects partial max from other processes
		int  maxtmp;
		for (i = 1; i < np; i++) {
      MPI_Recv(&maxtmp, 1, MPI_INT,
					i, 0,
					MPI_COMM_WORLD,
					&status);
      MPI_Recv(&message, 1000, MPI_CHAR,
					i, 0,
					MPI_COMM_WORLD,
					&status);
      printf("%s", message);
			int sender = status.MPI_SOURCE;
      if(maxtmp > max){
        max = maxtmp;
      }
		}
    int ndx;
    for(i=0; i<sz; i++){
      if(arr[i] == max){
        ndx = i;
        break;
      }
    }
		// prints the final sum of array
    printf("Master process announce the final max which is %d and its index is %d.\n", max, ndx);
	printf("Thanks for using our program\n");
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
    int partialMax = arr2[0];
    int indx = 0;
    for (i = 1; i < n_elements_recieved; i++){
      if(arr2[i] > partialMax){
        partialMax = arr2[i];
        indx = i;
      }
    }
      
		sprintf(message,"Hello from slave#%d Max number in my partition is %d and index is %d. ", pid, partialMax, indx) ;

    MPI_Send(&partialMax, 1, MPI_INT,
				0, 0, MPI_COMM_WORLD);
    MPI_Send(&message, strlen(message), MPI_CHAR,
				0, 0, MPI_COMM_WORLD);
	}

	// cleans up all MPI state before exit of process
	MPI_Finalize();

	return 0;
}

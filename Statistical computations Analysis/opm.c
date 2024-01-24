#include <stdlib.h>   //malloc and free
#include <stdio.h>    //printf
#include <omp.h>      //OpenMP
#include<math.h>

// Very small values for this simple illustrative example
#define ARRAY_SIZE 8192   //Size of arrays whose elements will be added together.


int main (int argc, char *argv[]) 
{
    omp_set_num_threads(32);
	int * a;
    double start, end, mean, variance, sumPow = 0.0;
    int sum = 0;
    int n = ARRAY_SIZE;                 // number of array elements
	int i;       // loop index
        
    // allocate spce for the array
	
    //  for(i=0;i<n;i++)     //fill the array with random numbers
    //     a[i]=rand()%100;

    printf("Please enter size of array...\n");
    scanf("%d", &n);
	a = (int*)malloc(n * sizeof(int));
    printf("Please enter array elements ...\n");
    for(i=0; i<n; i++){
    	scanf("%d", &a[i]);
    }

    printf("\n");
    start = omp_get_wtime();
	#pragma omp parallel for
    for(i = 0; i < n; i++) 
	{
		sum += a[i];
    }
	
    mean = (double) sum / (double) n;
    printf("mean = %f\n", mean);

    #pragma omp parallel for
    for(i = 0; i < n; i++) 
	{
		sumPow += pow((a[i]-mean), 2);
    }
    variance = sumPow / (double) n;
    printf("variance = %f\n", variance);
    printf("Standard deviation = %f\n", sqrt(variance));
	end = omp_get_wtime();
	printf( "\nElapsed time is %f in Seconds.", end - start); 
    // clean up memory
    free(a);
    return 0;
}
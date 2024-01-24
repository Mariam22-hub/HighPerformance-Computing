#include <stdio.h>
#include <mpi.h>
#include <stdbool.h>
#include <math.h>
int total_count;
int main(int argc, char** argv)
{
    int my_rank, comm_size;
    int x, y, r, a, b, count = 0, partial_count = 0;
    double end, start;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    if (my_rank == 0) {
        // Master process reads input range x and y from user
        printf("Enter lower bound (x): ");
        scanf("%d", &x);
        printf("Enter upper bound (y): ");
        scanf("%d", &y);

        // Calculate the subrange size for each process
        r = (y - x) / (comm_size - 1);    //20 - 5 /  3 = 5
        int remainder = (y - x) % (comm_size - 1); //1

        if (remainder > 0) {
            // Handle remaining numbers in master process
            int j, cn, k;
            for (j = y - remainder; j < y; j++) {
                cn = 1;
                for (k = 2; k <= j; k++) {
                    if (j % k == 0) {
                        cn++;
                    }
                }
                if (cn == 2) {
                    count++;
                }
            }
        }
        // Broadcast x and r to each slave process
        MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);



        MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


        printf("Total count of prime numbers between %d and %d: %d\n", x, y, total_count);

    }
    else {
        int a, b, total_count;
        MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);


        b = x + (r * my_rank) - 1;
        a = (b - r) + 1;
        int j, cn, k;
        for (j = a; j <= b; j++) {
            cn = 1;
            for (k = 2; k <= j; k++) {
                if (j % k == 0) {
                    cn++;
                }
            }

            if (cn == 2) {
                count++;
            }
        }

        MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    MPI_Finalize();
    if (my_rank == 0) {
        printf("Runtime = %f\n", end - start);
    }

    return 0;
}
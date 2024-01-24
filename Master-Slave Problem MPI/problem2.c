#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"


int **matrix_A, **matrix_B,**result, rowA, columnA, rowB, columnB, data, i, j,h,k, extra;
void free_2d_matrix (int **mat)
{
  free (mat[0]);
  free (mat);
}
int **matAlloc (int row, int col)
{
    int* x;
    int** mat;
    int count = 0;
    x = malloc(sizeof(int)*row *col);
    mat = malloc(sizeof(int*)*row);
    for(count = 0; count<row; count++)
       mat[count] = &x[count * col];
return mat;
}

int **readA(int r, int c, const char* filename){
    int **mat;
    int j, k;
    FILE *file = fopen(filename, "r");
    int i=0;
    
    scanf(file, "%d", &r);
    scanf(file, "%d", &c);
    columnA = i;
    mat = matAlloc(r, c);
    
        // printf("%d", i);
        for(j=0; j<r; j++){
            for(k=0; k<c; k++){
                scanf(file, "%d", &mat[j][k]);
            }
        }
    fclose(file);
return mat;
}

int main(int argc , char * argv[])
{
    int my_rank;
	int p;
	int source;
	int tag = 0;
	MPI_Status status;
    int dest;


	MPI_Init( &argc , &argv );

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	MPI_Comm_size(MPI_COMM_WORLD, &p);

  //slaves

 if(my_rank==0)
  {
	int choice;
     printf("Welcome to vector Matrix multiplication program!\n");
	 printf("To read dimensions and values from file press 1\nTo read dimensions and values from console press 2\n");
	 scanf("%d", &choice);
	 if(choice == 2){

		printf("Please enter dimensions of the first matrix: ");
            scanf("%d %d", &rowA, &columnA);
            
            matrix_A = matAlloc(rowA,columnA);
			
            printf("Please enter its elements:\n");
            
            for(i = 0; i < rowA; i++){
               for(j = 0 ; j < columnA; j++){
                 scanf("%d", &data);
                 matrix_A[i][j] = data;
              }}

            printf("\nPlease enter dimensions of the second matrix: ");
            
            scanf("%d %d", &rowB, &columnB);
            
            matrix_B = matAlloc(rowB,columnB);
			
            printf("Please enter its elements:\n");
            
            for(i = 0; i < rowB; i++){
              for(j = 0 ; j < columnB; j++){
                 scanf("%d", &data);
                 matrix_B[i][j] = data;
             }}

	 }
     if(choice == 1){
        matrix_A = read(rowA, columnA, "matrix1.txt");
        matrix_B = readB(rowB, columnB, "matrix2.txt");
     }
		
		int remaining_rows;
		int size_matRes;
		int slave_row = rowA/(p-1);
		remaining_rows = rowA%(p-1);
		size_matRes = slave_row*columnB;
    //---------------------------------------------------------------------------------------
    // first possibility p = no. of rows for each slave
         int k = 0;
         
         for( dest = 1; dest < p ; dest++)
         {
            //sending row of first matrix and matrix 2
            MPI_Send(&columnA, 1, MPI_INT, dest, tag, MPI_COMM_WORLD); //number of cols for each slave
            MPI_Send(&slave_row, 1, MPI_INT, dest, tag, MPI_COMM_WORLD); //number of rows for each slave
            MPI_Send(&columnB, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&rowB, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&size_matRes, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&matrix_A[k][0],slave_row*columnA,MPI_INT, dest,tag, MPI_COMM_WORLD);
            MPI_Send(&matrix_B[0][0],rowB*columnB,MPI_INT, dest,tag, MPI_COMM_WORLD);
            k+=slave_row;
         }


			
            //receiving the product matrix
            k = 0;
            result = matAlloc(rowA,columnB);
            for( dest = 1; dest < p; dest++)
            {
                MPI_Recv(&size_matRes, 1, MPI_INT, dest, tag, MPI_COMM_WORLD,&status);
                MPI_Recv(&result[k][0], slave_row*columnB, MPI_INT, dest, tag, MPI_COMM_WORLD,&status);
                k+=slave_row;
            }
            if (remaining_rows != 0){    //second possibility
             for(i = 0; i < remaining_rows ; i++){
                for (j = 0; j <columnB; j++) {
                 for (h = 0; h <rowB; h++){
                    result[k][j] += (matrix_A[k][h] * matrix_B[h][j]);
                 }}
                 k++;
        }
        }
         printf("Result matrix %d x %d is \n", rowA,columnB);
        for (i=0; i<rowA; i++){
            for(j=0; j<columnB; j++){
                printf("%d ", result[i][j]);
            }
            printf("\n");
        }
}

  else
  {
        int x;
        dest = 0;
        source = 0;
        int slave_row;
        int size_matRes = slave_row*columnB;
        MPI_Recv(&columnA, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&slave_row, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&columnB, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&rowB, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        matrix_A = matAlloc(slave_row,columnA);
        matrix_B = matAlloc(rowB,columnB);
        MPI_Recv(&size_matRes, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        result = matAlloc(slave_row,columnB);
        MPI_Recv(&(matrix_A[0][0]), columnA*slave_row, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&(matrix_B[0][0]), rowB*columnB , MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        
		for (i = 0; i< slave_row; i++)
            for (j=0; j< columnB; j++)
                result[i][j] = 0;

        for (i = 0; i <slave_row; i++)
            for (j = 0; j <columnB; j++)
                for (k = 0; k <rowB; k++)
                    result[i][j] += (matrix_A[i][k] * matrix_B[k][j]);

        //sending back to the master
        MPI_Send(&size_matRes, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
        MPI_Send(&result[0][0],slave_row*columnB, MPI_INT, source, tag, MPI_COMM_WORLD);
    }

  //master
 
    MPI_Finalize();
    free_2d_matrix(matrix_A);
    free_2d_matrix(matrix_B);
    /* shutdown MPI */

	return 0;

}

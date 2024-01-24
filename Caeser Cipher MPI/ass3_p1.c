#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


char* read_file(char* filename);
char encrypt(char c, int shift);

int len;


int main(int argc, char* argv[])
{
            
    char* text;

    int my_rank, no_of_p, chunkSize, key, i;
    char* output_text;
    char* root_output;
    MPI_Status status;

    // Creation of parallel processes
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_p);
 
    char ch;

    if (my_rank == 0) {
        printf("Enter the key: ");
        scanf("%d", &key);

        printf("---before reading the file---\n");

        text = read_file("input.txt");
        printf("original text: %s \n" , text);
        len = strlen(text);

        root_output = (char*)malloc(sizeof(char) * len + 1);

        printf("length of message: %d\n", len);

        if (text == NULL) {
            printf("error reading file");
            MPI_Finalize();
            return 0;
        }

        printf("no of processes: %d\n", no_of_p);

        printf("rank: %d\n", my_rank);
        
        chunkSize = (len) / (no_of_p);
        printf("chunk size: %d \n", chunkSize);


    }

    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    output_text = (char*)malloc(sizeof(char*) * chunkSize + 1);
    
    MPI_Scatter(text, chunkSize, MPI_CHAR, output_text, chunkSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    printf("output text before loop: %s \n", output_text);

    for (i = 0; i < chunkSize; i++) {
        printf("rank: %d\n", my_rank);
        
        printf("letter before encryption: %c \n", output_text[i]);
        
        output_text[i] = encrypt(output_text[i], key);
        
        printf("letter after encryption: %c \n", output_text[i]);
    }

    if (my_rank == 0 && strlen(text) % no_of_p != 0) {
        for (i = chunkSize * no_of_p; i < strlen(text); i++) {
            root_output[i] = encrypt(text[i], key);
        }
    }

    MPI_Gather(output_text, chunkSize, MPI_CHAR, root_output, chunkSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        printf("text after encryption is: %s\n", root_output);
        printf("text size after encryption is: %d\n", len);
        FILE* fp = fopen("Ciphertext.txt", "w");
        fputs(root_output, fp);
        fclose(fp);
    }


    //free(output_text);
    //free(root_output);
    //free(text);
    MPI_Finalize();


}

char* read_file(char *filename) {
    
    FILE* file;

    file = fopen(filename, "r");

    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    //printf("original character size: %d\n", len);
    fseek(file, 0, SEEK_SET);

    char* string = (char*)malloc(sizeof(char) * len + 1);

    char c; 
    int i = 0;

    fread(string, len+1, 1, file);
    string[len] = '\0';

    fclose(file);

    return string;
}

char encrypt(char c, int shift) {
    if (c >= 'A' && c <= 'Z') {
        return (c - 'A' + shift) % 26 + 'A';
    }
    else if (c >= 'a' && c <= 'z') {
        return (c - 'a' + shift) % 26 + 'a';
    }
    else {
        return c;
    }
}

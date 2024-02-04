#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 2048
#define PROCESSES 8

void initialize_matrix(int matrix1[MATRIX_SIZE][MATRIX_SIZE], int matrix2[MATRIX_SIZE][MATRIX_SIZE]);
void write_file(int sum[MATRIX_SIZE][MATRIX_SIZE]);

int matrix1[MATRIX_SIZE][MATRIX_SIZE],
  matrix2[MATRIX_SIZE][MATRIX_SIZE],
  sum[MATRIX_SIZE][MATRIX_SIZE];

int main(int argc, char* argv[]) {
  int p_rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

  int matrix1_local[MATRIX_SIZE / PROCESSES][MATRIX_SIZE],
    matrix2_local[MATRIX_SIZE / PROCESSES][MATRIX_SIZE],
    sum_local[MATRIX_SIZE / PROCESSES][MATRIX_SIZE];
  struct timespec begin, end;

  MPI_Request requests[3] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL};
  MPI_Status status;
  if (p_rank == 0) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    initialize_matrix(matrix1, matrix2);
  }

  MPI_Iscatter(&matrix1, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT,
    &matrix1_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD, &requests[0]);
  MPI_Iscatter(&matrix2, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT,
    &matrix2_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD, &requests[1]);

  MPI_Wait(&requests[0], &status);
  MPI_Wait(&requests[1], &status);
  int i, j;
  for (i = 0; i < MATRIX_SIZE / PROCESSES; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      sum_local[i][j] = matrix1_local[i][j] + matrix2_local[i][j];
    }
  }

  printf("Process %d: Done\n", p_rank);

  MPI_Igather(&sum_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT,
    &sum, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD, &requests[2]);
  
  if (p_rank == 0) {
    MPI_Wait(&requests[2], &status);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf ("Total time = %f seconds\n",
      (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
      (end.tv_sec  - begin.tv_sec));
    write_file(sum);
  }

  MPI_Finalize();
}

void initialize_matrix(int matrix1[MATRIX_SIZE][MATRIX_SIZE], int matrix2[MATRIX_SIZE][MATRIX_SIZE]) {
  int i, j;
  for (i = 0; i < MATRIX_SIZE; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      matrix1[i][j] = j + 1;
      matrix2[i][j] = j + 1;
    }
  }
}

void write_file(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
  FILE *fp1;

  fp1 = fopen("./result.txt", "w+");

  int i, j;
  for(i = 0; i < MATRIX_SIZE; i++) {
    for(j = 0; j < MATRIX_SIZE; j++) {
      fprintf(fp1, "%d ", matrix[i][j]);
    }
    fprintf(fp1, "\n");
  }

  fclose(fp1);
}

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 2048

void initialize_matrix(int matrix1[MATRIX_SIZE][MATRIX_SIZE], int matrix2[MATRIX_SIZE][MATRIX_SIZE]);
void write_file(int sum[MATRIX_SIZE][MATRIX_SIZE]);

int matrix1[MATRIX_SIZE][MATRIX_SIZE],
  matrix2[MATRIX_SIZE][MATRIX_SIZE],
  sum[MATRIX_SIZE][MATRIX_SIZE];

int main(int argc, char* argv[]) {
  struct timespec begin, end;

  clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
  initialize_matrix(matrix1, matrix2);

  int i, j;
  for (i = 0; i < MATRIX_SIZE; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      sum[i][j] = matrix1[i][j] + matrix2[i][j];
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf ("Total time = %f seconds\n",
    (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
    (end.tv_sec  - begin.tv_sec));
  write_file(sum);
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

  fp1 = fopen("./result_serial.txt", "w+");

  int i, j;
  for(i = 0; i < MATRIX_SIZE; i++) {
    for(j = 0; j < MATRIX_SIZE; j++) {
      fprintf(fp1, "%d ", matrix[i][j]);
    }
    fprintf(fp1, "\n");
  }

  fclose(fp1);
}

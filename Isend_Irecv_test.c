  int i, j;
  MPI_Request requests[PROCESSES * 2];
  MPI_Status statuses[PROCESSES * 2];
  for (i = 0; i < PROCESSES * 2; i++) {
    requests[i] = MPI_REQUEST_NULL;
  }

  if (p_rank == 0) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    initialize_matrix(matrix1, matrix2);

    for (i = 0; i < PROCESSES; i++) {
      MPI_Isend(&matrix1[(MATRIX_SIZE / PROCESSES) * i][0], (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
        MPI_INT, i, 1, MPI_COMM_WORLD, &requests[i]);
      MPI_Isend(&matrix2[(MATRIX_SIZE / PROCESSES) * i][0], (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
        MPI_INT, i, 2, MPI_COMM_WORLD, &requests[i + PROCESSES]);
      // MPI_Send((matrix1 + (MATRIX_SIZE / PROCESSES) * i), (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      //   MPI_INT, i, 1, MPI_COMM_WORLD);
      // MPI_Send((matrix2 + (MATRIX_SIZE / PROCESSES) * i), (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      //   MPI_INT, i, 2, MPI_COMM_WORLD);
    }
  }

  for (i = 0; i < PROCESSES; i++) {
    // MPI_Irecv(&matrix1_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
    //   MPI_INT, 0, 1, MPI_COMM_WORLD, &requests[i]);
    // MPI_Irecv(&matrix2_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
    //   MPI_INT, 0, 2, MPI_COMM_WORLD, &requests[i + PROCESSES]);
    MPI_Recv(&matrix1_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      MPI_INT, 0, 1, MPI_COMM_WORLD, &statuses[i]);
    MPI_Recv(&matrix2_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      MPI_INT, 0, 2, MPI_COMM_WORLD, &statuses[i + PROCESSES]);
  }

  for (i = 0; i < PROCESSES * 2; i++) {
    MPI_Wait(&requests[i], &statuses[i]);
  }

  for (i = 0; i < MATRIX_SIZE / PROCESSES; i++) {
    for (j = 0; j < MATRIX_SIZE; j++) {
      sum_local[i][j] = matrix1_local[i][j] + matrix2_local[i][j];
    }
  }
  printf("Process %d: Done\n", p_rank);

  for (i = 0; i < PROCESSES; i++) {
    MPI_Isend(&sum_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      MPI_INT, 0, 3, MPI_COMM_WORLD, &requests[i]);
    // MPI_Send(&sum_local, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
    //   MPI_INT, 0, 3, MPI_COMM_WORLD);
  }

  if (p_rank == 0) {
    for (i = 0; i < PROCESSES; i++) {
      // MPI_Irecv(&sum + ((MATRIX_SIZE / PROCESSES) * MATRIX_SIZE) * i, (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
      //   MPI_INT, i, 3, MPI_COMM_WORLD, &requests[i]);
      MPI_Recv(&sum[(MATRIX_SIZE / PROCESSES) * i][0], (MATRIX_SIZE / PROCESSES) * MATRIX_SIZE,
        MPI_INT, i, 3, MPI_COMM_WORLD, &statuses[i]);
    }
    for (i = 0; i < PROCESSES; i++) {
      MPI_Wait(&requests[i], &statuses[i]);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf ("Total time = %f seconds\n",
      (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
      (end.tv_sec  - begin.tv_sec));
    write_file(sum);
  }
/*
 * Swarthmore College, CS 87
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

/*
 * TODO: add top-level comments, your names, and date
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>
#include <string.h>


/* the **tiny** default number of data elements in each process
 * (can change or use command line option to run for larger sizes)
 */
//#define SIZE 10
#define SIZE 100

/* the default largest data value for random generation of values
 * (feel free to change this if you'd like)
 */
//#define MAX 1000
#define MAX 1000

/* Here are some debug PRINT macros you can use
 * add more like this if you want to call printf with more than two args
 *
 * uncomment DEBUG definition to turn on  debug printing
 * comment   DEBUG definition to turn off debug printing
 */


#define DEBUG

#ifdef DEBUG
#define PRINT0(s)     printf(s)
#define PRINT1(s,a)   printf(s,a)
#define PRINT2(s,a,b) printf(s,a,b)
#else
#define PRINT0(s)
#define PRINT1(s,a)
#define PRINT2(s,a,b)
#endif

/* TODO: add function prototypes here. do not use global variables */

#include <stdio.h>
#include <stdlib.h>
int check_values(int* values, int size){
  for(int i = 1; i<size-1;i++){
    if(values[i]<values[i-1]){
      printf("Error values: %d %d", values[i], values[i-1]);
      return -1;
    }
  }
  return 0;
}



int cmpfunc (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

void keep_smallest(int *values, int *received, int size) {
  int *temp = malloc(sizeof(int) * size * 2);

  memcpy(temp, values, size * sizeof(int));
  memcpy(&temp[size], received, size * sizeof(int));

  qsort(temp, size * 2, sizeof(int), cmpfunc);

  memcpy(values, temp, size * sizeof(int));
  free(temp);
}

void keep_biggest(int *values, int *received, int size) {
  int *temp = malloc(sizeof(int) * size * 2);

  memcpy(temp, values, size * sizeof(int));
  memcpy(&(temp[size]), received, size * sizeof(int));

  qsort(temp, size * 2, sizeof(int), cmpfunc);

  memcpy(values, &temp[size], size * sizeof(int));
  free(temp);
}

int oddevensort(int* values, int *received, int size, int rank, int num_threads) {
  MPI_Status status;

  for (int r = 0; r < 2 * num_threads; r++) {
    qsort(values, size, sizeof(int), cmpfunc);
    //if even round
    if (r % 2 == 0) {
      //if the thread/pi is even
      if (rank % 2 == 0 && rank < num_threads - 1) {
        //it keeps its first half and sends its second half to pi+1 thread
        int ret = MPI_Send(values, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        ret = MPI_Recv(received, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
        if (ret != MPI_SUCCESS){
          perror("MPI send was not succesful\n");
          return -1;
        }

        keep_smallest(values, received, size);
      }

      if (rank % 2 == 1 && rank > 0) {
        //if odd, it recieves values from pi-1 first(to avoid gridlock)
        int ret = MPI_Recv(received, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        //Once its recieving function is complete, it sends its own values to
        //pi-1
        ret = MPI_Send(values, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        keep_biggest(values, received, size);
      }
    } else {
      //if we are at an odd round
      //if even
      if (rank % 2 == 0 && rank > 0) {
        //it keeps its first half and sends its second half to pi+1 thread
        int ret= MPI_Send(values, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        ret= MPI_Recv(received, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        keep_biggest(values, received, size);
      }

      if (rank % 2 == 1 && rank < num_threads - 1) {
        //if odd, it recieves values from pi+1 first(to avoid gridlock)
        int ret= MPI_Recv(received, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        //Once its recieving function is complete, it sends its own values to
        //pi-1
        ret = MPI_Send(values, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
          perror("MPI send was not succesful\n");
          return -1;
        }
        keep_smallest(values, received, size);
      }
    }
    // MPI_Barrier(MPI_COMM_WORLD);
  }

  return 0;
}

void print_values(int* values, int size, int rank, int num_threads) {
  MPI_Barrier(MPI_COMM_WORLD);

  for (int i = 0; i < num_threads; i++) {
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == i) {
      printf("Process %d: ", rank);
      for (int i = 0; i < size; i++) {
        printf("%3d ", values[i]);
      }
      PRINT0("\n");
      fflush(stdout);
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    PRINT0("\n");
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);
}

/*****************************************************/
int main(int argc, char** argv) {
  int num_threads = 0;
  int rank = 0;
  int size = SIZE;
  int repetitions = 1;
  char hostname[1024];


  if (argc > 3) {
    printf("usage: ./oddevensort (size) (iters)\n");
    exit(0);
  }

  if (argc == 2) {
    size = atoll(argv[1]);
  }

  if (argc == 3) {
    size = atoll(argv[1]);
    repetitions = atoll(argv[2]);
  }

  if (size < 1) {
    printf("size must be at least 1\n");
    exit(1);
  }

  if (repetitions < 1) {
    printf("repetitions must be at least 1\n");
    exit(1);
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  gethostname(hostname, 1024);

  //for each thread, we use a simple function to generate a seed
  srand(rank + time(NULL));

  int* array = malloc(sizeof(int) * size);
  int* received = malloc(sizeof(int) * size);

  for (int rep = 0; rep < repetitions; rep++) {
    MPI_Barrier(MPI_COMM_WORLD);

    // generate the array for each thread
    for (int i = 0; i < size; i++) {
      array[i] = rand() % MAX;
    }

#ifdef DEBUG
    print_values(array, size, rank, num_threads);
#endif

    oddevensort(array, received, size, rank, num_threads);

#ifdef DEBUG
    check_values(array, size);
    print_values(array, size, rank, num_threads);
#endif

    MPI_Barrier(MPI_COMM_WORLD);
  }

  free(array);
  free(received);
  MPI_Finalize();
  return 0;
}

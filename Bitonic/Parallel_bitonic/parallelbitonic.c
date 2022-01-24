#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include "parallelbitonic.h"

#define MASTER 0
#define OUTPUT_NUM 10

/* This code was adapted from https://cse.buffalo.edu/faculty/miller/Courses/CSE702/Sajid.Khan-Fall-2018.pdf */




// Globals
double timer_start;
double timer_end;
int rank;
int num_processes;
int * array;
int size;


//prints out the different values in the array
void print_values(int* values, int size, int rank, int num_threads) {
  MPI_Barrier(MPI_COMM_WORLD);

  for (int i = 0; i < num_threads; i++) {
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == i) {
      printf("Process %d: ", rank);
      for (int i = 0; i < size; i++) {
        printf("%3d ", values[i]);
      }
      printf("\n");
      fflush(stdout);
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    printf("\n");
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);
}






//input is the final size
int main(int argc, char** argv){
    int i=0;
    int j= 0;
    char hostname[1024];
    size = atoi(argv[1]);
    array = NULL;

    //optional input's default value
    int debug = 0;

    if(argc==3){
      debug = atoi(argv[2]);
    }
    if(argc==4){
      num_processes =atoi(argv[3]);
      debug = atoi(argv[2]);
    }
    if(argc>4){
      printf("usage: ./bitonic (size) (debug mode(default is False)) (number of mpi proceses)\n");
      exit(0);
    }



    // Initialization, get # of processes & this PID/rank
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    gethostname(hostname, 1024);

    // Initialize Array for Storing Random Numbers
    array = (int *) malloc(size * sizeof(int));
    if(array==NULL){
      printf("error in mallocing the array\n");
      exit(1);
    }

    // Generate Random Numbers for Sorting (within each process)
    // Less overhead without MASTER sending random numbers to each slave
    srand(rank+time(NULL));  // Needed for rand()
    for (i = 0; i < size; i++) {
        array[i] = rand() % 1000;
    }
    if(debug==1){
      print_values(array,size,rank,num_processes);
    }
    // Blocks until all processes have finished generating
    MPI_Barrier(MPI_COMM_WORLD);


    // Cube Dimension
    int dimensions = (int)(log2(num_processes));

    // Start Timer before starting first sort operation (first iteration)
    if (rank == MASTER && debug == 1) {
        printf("Number of Processes spawned: %d on %s\n", num_processes, hostname);
        timer_start = MPI_Wtime();
    }

    // Sequential Sort
    qsort(array, size, sizeof(int), ComparisonFunc);

    // Bitonic Sort follows
    for (i = 0; i < dimensions; i++) {
        for (j = i; j >= 0; j--) {
            // (window_id is even AND jth bit of process is 0)
            // OR (window_id is odd AND jth bit of process is 1)
            if (((rank >> (i + 1)) % 2 == 0 && (rank >> j) % 2 == 0) || ((rank >> (i + 1)) % 2 != 0 && (rank >> j) % 2 != 0)) {
                CompareLow(j);
            } else {
              CompareHigh(j);
            }
        }
    }


    // Blocks until all processes have finished sorting
    MPI_Barrier(MPI_COMM_WORLD);
    if(debug==1){
      print_values(array,size,rank,num_processes);
    }
    if (rank == MASTER && debug==1) {
        timer_end = MPI_Wtime();
        printf("\n\n");
        printf("Time Elapsed (Sec): %f\n", timer_end - timer_start);
    }

    // free memory
    free(array);

    // Done
    MPI_Finalize();

    //printf("\n\nFINISHING SUCCESSFULLY ALHAMDULILLAH");
    return 0;
}

/*comparison function for quick sort library)*/
int ComparisonFunc(const void * a, const void * b) {
    return ( * (int *)a - * (int *)b );
}

void CompareLow(int j) {
    int i, min;

   // printf("My rank is %d Pairing with %d in CL\n", rank, rank^(1<<j));

    int send_counter = 0;
    int *buffer_send = malloc((size + 1) * sizeof(int));
   // printf("Trying to send local max in CL:%d\n", array[size-1]);
   //sends local maximum to paired processor
    MPI_Send(
        &array[size - 1],
        1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    int recv_counter;
    int *buffer_recieve = malloc((size + 1) * sizeof(int));
    //recieves the minimum from paired processor
    MPI_Recv(
        &min,
        1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

   // printf("Received min from pair in CL:%d\n", min);
   //finds the appropriate elements to send which are larger
   //than the current paried processor's minimum
    for (i = size-1; i >= 0; i--) {
        if (array[i] > min) {
	    send_counter++;
            buffer_send[send_counter] = array[i];
	   // printf("Buffer sending in CL %d\n", array[i]);

        } else {
             break;
        }
    }

    buffer_send[0] = send_counter;
    //send the
    MPI_Send(
        buffer_send,
        send_counter+1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    MPI_Recv(
        buffer_recieve,
        size+1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );
    //creates a temporary array to store recieved values
    int *temp_array = (int *) malloc(size * sizeof(int));
    //memcpy(temp_array, array, size * sizeof(int));
    for(i=0;i<size;i++){
      temp_array[i]=array[i];
    }

    int buffer_size=buffer_recieve[0];
    int k=1;
    int m=0;


  //copies over the right values
  for (i = 0; i < size; i++) {

	   if(temp_array[m]<=buffer_recieve[k]){
		     array[i]=temp_array[m];
		     m++;
	      }
     else if(k<=buffer_size){
		      array[i]=buffer_recieve[k];
		      k++;
	 }
  }
//sorts them locally
    qsort(array, size, sizeof(int), ComparisonFunc);


    free(buffer_send);
    free(buffer_recieve);

    return;
}


void CompareHigh(int j) {

    //printf("My rank is %d Pairing with %d in CH\n", rank, rank^(1<<j));
    int i, max;

    int recv_counter;
    int *buffer_recieve = malloc((size + 1) * sizeof(int));
    //recieves the paired processor's maximum value
    MPI_Recv(
        &max,
        1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

   // printf("Received max from pair in CH:%d\n",max);
    int send_counter = 0;
    int *buffer_send = malloc((size + 1) * sizeof(int));
    //sends its local minimum to paired processor
    MPI_Send(
        &array[0],
        1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

   // printf("Sending min to my pair from CH:%d\n", array[0]);
   //copies over the correct elements
    for (i = 0; i < size; i++) {
        if (array[i] < max) {
	   // printf("Buffer sending in CH: %d\n", array[i]);
          send_counter++;
		      buffer_send[send_counter] = array[i];
        }
        else {
            break;
          }
    }
    //recieves the buffer of values from paired processor
    MPI_Recv(
        buffer_recieve,
        size+1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );
    recv_counter = buffer_recieve[0];

    buffer_send[0] = send_counter;
    //printf("Send counter in CH: %d\n", send_counter);

  //  for(i=0;i<=send_counter;i++)
//	printf(" %d>> ", buffer_send[i]);
    //sends its buffer values to the paired processor
    MPI_Send(
        buffer_send,
        send_counter+1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );
    int *temp_array = (int *) malloc(size * sizeof(int));
    //memcpy(temp_array, array, size * sizeof(int));
    for(i=0;i<size;i++){
	   temp_array[i]=array[i];
    }
    int k=1;int m=size-1;
    int buffer_size=buffer_recieve[0];


    for (i = size-1; i >= 0; i--) {

	     if(temp_array[m]>=buffer_recieve[k])
	      {
		        array[i]=temp_array[m];
		          m--;
	           }
	      else if(k<=buffer_size){
		        array[i]=buffer_recieve[k];
		        k++;
	         }
    }

    qsort(array, size, sizeof(int), ComparisonFunc);


    //printf("\n");
    free(temp_array);
    free(buffer_send);
    free(buffer_recieve);

    return;
}

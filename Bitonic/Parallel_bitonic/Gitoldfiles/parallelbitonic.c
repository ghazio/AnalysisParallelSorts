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

// Globals
// Not ideal for them to be here though
double timer_start;
double timer_end;
int rank;
int num_processes;
int * array;
int size;

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




///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////

//input is the final size, it should be divisible by
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
    int *temp_array = (int *) malloc(size * sizeof(int));
    //memcpy(temp_array, array, size * sizeof(int));
    for(i=0;i<size;i++){
      temp_array[i]=array[i];
    }

    int buffer_size=buffer_recieve[0];
    int k=1;
    int m=0;



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

    qsort(array, size, sizeof(int), ComparisonFunc);
  //  for(i=0;i<size;i++)
//	printf("My rank is %d, after exchange in CL %d\n", rank, array[i]);

  // int s=0;
   //for(i=0;i<size;i++)

//	{
//		printf("%d", array[i]);
//		for(s=0;s<=rank;s++)
//			printf(":");
//	}

//	printf("\n");

    free(buffer_send);
    free(buffer_recieve);

    return;
}
/*
void CompareLow(int j) {
    int i = 0;
    int* min = malloc(sizeof(int)*1);
     Sends the biggest of the list and receive the smallest of the list

    // Send entire array to paired H Process
    // Exchange with a neighbor whose (d-bit binary) processor number differs only at the jth bit.
    int send_counter = 0;
    int * buffer_send = malloc((size + 1) * sizeof(int));
    if(buffer_send==NULL){
      printf("error in buffer send allocation\n");
      exit(1);
    }

    int ret =MPI_Send(&array[size - 1], 1,MPI_INT,rank ^ (1 << j),0,MPI_COMM_WORLD);
    if(ret!=0){
      printf("error in mpi send\n");
      exit(1);
    }

    int recv_counter;
    int* buffer_recieve = malloc((size + 1) * sizeof(int));
    if(buffer_recieve==NULL){
      printf("error in buffer send allocation\n");
      exit(1);
    }
    MPI_Recv(min,  1,MPI_INT,rank ^ (1 << j), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Buffers all values which are greater than min send from H Process.
    for (i = 0; i < size; i++) {
        if (array[i] > *min) {
            buffer_send[send_counter + 1] = array[i];
            send_counter++;
        } else {
            break;      // Important! Saves lots of cycles!
        }
    }

    buffer_send[0] = send_counter;

    // send partition to paired H process
    ret = MPI_Send(buffer_send,send_counter,MPI_INT,rank ^ (1 << j),0,MPI_COMM_WORLD);
    if(ret != 0){
      printf("error in MPI recieve in compare high\n");
      exit(1);
    }

    // receive info from paired H process
    ret = MPI_Recv(buffer_recieve,size,MPI_INT,rank ^ (1 << j),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    if(ret != 0){
      printf("error in MPI recieve in compare high\n");
      exit(1);
    }
    // Take received buffer of values from H Process which are smaller than current max
    for (i = 1; i < buffer_recieve[0] + 1; i++) {
        if (array[size - 1] < buffer_recieve[i]) {
            // Store value from message
            array[size - 1] = buffer_recieve[i];
        } else {
            break;
        }
    }



    // Sequential Sort
    qsort(array, size, sizeof(int), ComparisonFunc);

    // Reset the state of the heap from Malloc
    free(buffer_send);
    free(buffer_recieve);

    return;
}
*/

void CompareHigh(int j) {

    //printf("My rank is %d Pairing with %d in CH\n", rank, rank^(1<<j));
    int i, max;

    int recv_counter;
    int *buffer_recieve = malloc((size + 1) * sizeof(int));

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
    MPI_Send(
        &array[0],
        1,
        MPI_INT,
        rank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

   // printf("Sending min to my pair from CH:%d\n", array[0]);
    for (i = 0; i < size; i++) {
        if (array[i] < max) {
	   // printf("Buffer sending in CH: %d\n", array[i]);
            	send_counter++;
		buffer_send[send_counter] = array[i];
        } else {
            break;
        }
    }


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
    for(i=0;i<size;i++)
	temp_array[i]=array[i];

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
    free(buffer_send);
    free(buffer_recieve);

    return;
}

/*
void CompareHigh(int j) {
    int i = 0;
    int* max = malloc(sizeof(int)*1);
    // Receive max from L Process's entire array
    int recv_counter;
    int * buffer_recieve = malloc((size + 1) * sizeof(int));
    if(buffer_recieve==NULL){
      printf("error in buffer send allocation\n");
      exit(1);
    }
    int ret = MPI_Recv(max,1,  MPI_INT, rank ^ (1 << j),  0,  MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
    if(ret!=0){
      printf("error in MPI recieve in compare high\n");
      exit(1);
    }
    // Send min to L Process of current process's array
    int send_counter = 0;
    int * buffer_send = malloc((size + 1) * sizeof(int));
    if(buffer_send==NULL){
      printf("error in buffer send allocation\n");
      exit(1);
    }
    ret= MPI_Send(&array[0],1,MPI_INT,  rank ^ (1 << j),  0,  MPI_COMM_WORLD);
    if(ret!=0){
      printf("error in MPI recieve in compare high\n");
      exit(1);
    }

    // Buffer a list of values which are smaller than max value
    for (i = 0; i < size; i++) {
        if (array[i] < *max) {
            buffer_send[send_counter + 1] = array[i];
            send_counter++;
        } else {
            break;
        }
    }

    // Receive blocks greater than min from paired slave
    ret = MPI_Recv(buffer_recieve,size,MPI_INT,rank ^ (1 << j), 0,  MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
    if(ret!=0){
      printf("error in second MPI recieve in compare high\n");
      exit(1);
    }
    recv_counter = buffer_recieve[0];

    // send partition to paired slave
    buffer_send[0] = send_counter;
    ret= MPI_Send(buffer_send,send_counter,  MPI_INT,rank ^ (1 << j),0,MPI_COMM_WORLD);
    if(ret!=0){
      printf("error in MPI recieve in compare high\n");
      exit(1);
    }
    // Take received buffer of values from L Process which are greater than current min
    for (i = 1; i < recv_counter + 1; i++) {
        if (buffer_recieve[i] > array[0]) {
            // Store value from message
            array[0] = buffer_recieve[i];
        } else {
            break;      // Important! Saves lots of cycles!
        }
    }

    // Sequential Sort
    qsort(array, size, sizeof(int), ComparisonFunc);

    // Reset the state of the heap from Malloc
    free(buffer_send);
    free(buffer_recieve);
    free(max);

    return;
}
*/

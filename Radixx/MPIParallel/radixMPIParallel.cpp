/* Swarthmore College Fall 2021
Parallel Radix Sort Implementation
Marshall, Ghazi, Beluchi */


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>
#include <cmath>
using namespace std;

int MAX = 1000;//max int value that can be stored in our array



//Function Prototypes
int getMax(int *arr, int n);//gets the max value of an array
void countSort(int *arr, int n, int exp);//sorts values via lsd
void print(int *arr, int n);//prints the values in an array
void radixsortParallel(int *arr, int rank, int size, int numthreads, int max);//runs parallel radix sort




// A utility function to get maximum value in arr[]
int getMax(int *arr, int n){
    int mx = arr[0];
    for (int i = 1; i < n; i++){
        if (arr[i] > mx){
            mx = arr[i];
        }
    }
    return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(int *arr, int n, int exp){
    int* output=(int*)malloc(sizeof(int)*n); // output array
    int i;
    int count[10] = { 0 };
    // Store count of occurrences in count[]
    for (i = 0; i < n; i++){
        count[(arr[i] / exp) % 10]++;
    }
    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++){
        count[i] += count[i - 1];
    }
    // Build the output array
    for (i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
    for (i = 0; i < n; i++){
        arr[i] = output[i];
    }

}

// The main function to that sorts arr[] of size n using
// Radix Sort
void radixsortParallel(int *arr, int rank, int size, int numthreads, int m){
  int num_digits = log10(m) + 1;
  int rounds = 0;
  int digit_check = 0;


  // Find the maximum number to know number of digits
  //I think this should have been MAX
  //m = getMax(arr, size);


  for (int exp = 1; m / exp > 0; exp *= 10){
    int* buffer= (int*) malloc(sizeof(int)*10*numthreads);
    int* newArr = new int [size * sizeof(int)];
    int digit_vals[10] = { 0 };
    int process_countsSum[10] = {0};//histogram for subinput array
    int process_prefixSum[10] = {0};//histogram for aggregate prefix sum array
    int process_sumLeft[10] = {0};//

    //FIRST PARALLEL STEP: use count sort for local sort of array vals
    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    countSort(arr, size, exp);



    //get the count sort histograms on each process
    for (int i = 0; i < size; i++){
      digit_vals[(arr[i] / exp) % 10]++;
    }


    //SECOND PARALLEL STEP: share info from local sort with other processes
    MPI_Allgather(digit_vals,10,MPI_INT, buffer,10,MPI_INT,MPI_COMM_WORLD);


    //THIRD PARALLEL STEP:get total digit counts of all processes and use them for redistribution
    /*parts of this redistribution step taken from Matthew Seligson on github's implementation
      found at https://github.com/mpseligson/radix/blob/master/radix.c */

    // sum up histograms into aggregate histogram allCountsSum
    for (int i = 0; i < 10 *numthreads; i++) {
      int lsd = i % 10;
      int process = i / 10;
      int current_bucket = buffer[i];

      // add histogram values to allCountsSumLeft for all processors "left" of current processor
      if (process < rank) {
          process_sumLeft[lsd] += current_bucket;
        }
        process_countsSum[lsd] += current_bucket;
        process_prefixSum[lsd] += current_bucket;
      }

    // build cumulative sum array
    for (int i = 1; i < 10; i++) {
      process_prefixSum[i] += process_prefixSum[i - 1];
      }


    // request and status variables to be passed to MPI send and receive functions
    MPI_Request request;
    MPI_Status status;

    // count of elements sent from current processor for each digit
    int lsdSent[10] = {0};

    // value, final position array to be sent between processors
    int valIndexPair[2];
    int current_bucket, lsd, destIndex, destProcess, localDestIndex;

    // send each element and receive each element
    for (int i = 0; i < size; i++) {
      current_bucket = arr[i];
      lsd = (arr[i] / exp) % 10;

      //printf("process_prefixSum %d\n", process_prefixSum[lsd]);
      //printf("process_countSum %d\n", process_countsSum[lsd]);
      //printf("process_sumleft %d\n", process_sumLeft[lsd]);


      // compute index of value in the aggregate array
      destIndex = process_prefixSum[lsd] - process_countsSum[lsd] + process_sumLeft[lsd] + lsdSent[lsd];

      // increment count of elements with key lsd is to be sent
      lsdSent[lsd]++;
      destProcess = destIndex / size;
      //printf("Destination index %d on rank %d: %d\n",destIndex,rank,process_prefixSum[lsd]);
      // set valIndexPair array to be sent
      valIndexPair[0] = current_bucket;
      valIndexPair[1] = destIndex;

      // send the pair, receive a pair from any processor into the same buffer as used to send
      MPI_Isend( &valIndexPair, 2, MPI_INT, destProcess, 0, MPI_COMM_WORLD, &request);
      MPI_Recv(valIndexPair, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      // compute index of value in local array and set the value
      localDestIndex = valIndexPair[1] % size;
      current_bucket = valIndexPair[0];
      newArr[localDestIndex] = current_bucket;
    }

    int tailOffset = 0;
    // set the values in subinput to the values in newSubinput
    for (int i = 0; i < size; i++) {
      arr[i] = newArr[i];
    }


  }//end of iterations for loop

}//end of parallel sort function

int check_values(int* values, int size){
  for(int i = 1; i<size-1;i++){
    if(values[i]<values[i-1]){
      printf("Error values: %d %d", values[i], values[i-1]);
      return -1;
    }
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

// A utility function to print an array
void print(int *arr, int n){
    for (int i = 0; i < n; i++){
        cout << arr[i] << " ";
    }
    printf("\n");
}

//inputs: size,
int main(int argc, char **argv){
    int numthreads = 0;
    int rank = 0;
    int max_val = 1000;
    int debug = 0;
    //Still need to determine arguments for this
    if (argc > 3) {
      printf("usage: ./radix sort (size) (debug)\n");
      exit(0);
    }

  //default: size of 100
    int size = 100;
    if (argc == 2) {
      size = atoll(argv[1]);
     }
    if(argc==3){
      size  = atoll(argv[1]);
      debug = atoll(argv[2]);
    }


   MPI_Init(&argc, &argv); // call MPIINit here look at the lab write up.
   MPI_Comm_rank(MPI_COMM_WORLD, (int*)&rank);
   MPI_Comm_size(MPI_COMM_WORLD, (int*)&numthreads);

   //this is a fix so that we do not have overflow errors
   int *array = new int [size];

   srand(rank+time(NULL));
   for(int i =0; i<size; i++){
     array[i] = rand() % MAX;  //place that number into the array;
   }

   if(debug==1){
     printf("Values should be printing\n");
     print_values(array,size,rank,numthreads);
   }
   //run parallel sorting algorithm
   radixsortParallel(array,rank,size,numthreads, MAX);

   if(debug==1){
     print_values(array,size,rank,numthreads);
   }
   if(debug==1){
    int ret = check_values(array, size);
    if(ret != 0){
      printf("There is an error in sorting in rank: %d\n",rank);
    }
  }
   //clean up threads and malloc'd OG array
   free(array);
   MPI_Finalize();

   return 0;
}

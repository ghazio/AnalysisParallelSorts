// C++ implementation of Radix Sort
/*
The sequential Radix sort code was provided by GeeksforGeeks and improved upon
by DrRoot_
rathbhupendra
m212076
becharaerizk
vishwajeet0524
sweetyty
simranarora5sos
KOMAL Y
unknown2108
sagar0719kumar
karthikns16
DishankJindal
before being utlized within this project. 
*/
#include <iostream>
using namespace std;

// A utility function to get maximum value in arr[]
int getMax(int arr[], int n)
{
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(int *arr, int n, int exp)
{
    int output[n]; // output array
    int i, count[10] = { 0 };

    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[(arr[i] / exp) % 10]++;

    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
    for (i = 0; i < n; i++)
        arr[i] = output[i];
}

// The main function to that sorts arr[] of size n using
// Radix Sort
void radixsort(int *arr, int n)
{
    // Find the maximum number to know number of digits
    int m = getMax(arr, n);

    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; m / exp > 0; exp *= 10)
        countSort(arr, n, exp);
}

// A utility function to print an array
void print(int *arr, int n)
{
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
}

//inputs: size,
int main(int argc, char **argv){
    if (argc > 2) {
    printf("usage: ./radix sort (size) \n");
    exit(0);
  }
  //default: size of 100
    int size = 100;
    if (argc == 2) {
        size = atoll(argv[1]);
     }


   //set the seed to 0
   srand(10*time(NULL));
   int randArray[size];
   for(int i=0;i<size;i++){
      randArray[i]=rand()%100;
  }
    int n = sizeof(randArray) / sizeof(randArray[0]);
    print(randArray,n);
    cout << "\nSorted Array\n";
    // Function Call
    radixsort(randArray, size);
    print(randArray, n);
    cout<< "\n";
    return 0;
}

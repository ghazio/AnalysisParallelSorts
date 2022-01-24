// A C++ Program to implement Odd-Even / Brick Sort
/*
This code was provided by GeeksforGeeks and enhanced by Sam007
splevel62 before being utlized within this project. 
*/
#include<bits/stdc++.h>
using namespace std;
#include <iostream>

// A function to sort the algorithm using Odd Even sort
void oddEvenSort(int arr[], int n)
{
    bool isSorted = false; // Initially array is unsorted

    while (!isSorted)
    {
        isSorted = true;

        // Perform Bubble sort on odd indexed element
        for (int i=1; i<=n-2; i=i+2)
        {
            if (arr[i] > arr[i+1])
             {
                swap(arr[i], arr[i+1]);
                isSorted = false;
              }
        }

        // Perform Bubble sort on even indexed element
        for (int i=0; i<=n-2; i=i+2)
        {
            if (arr[i] > arr[i+1])
            {
                swap(arr[i], arr[i+1]);
                isSorted = false;
            }
        }
    }

    return;
}

// A utility function ot print an array of size n
void printArray(int arr[], int n)
{
   for (int i=0; i < n; i++)
       cout << arr[i] << " ";
   cout << "\n";
}

// Driver program to test above functions.
int main(int argc, char **argv)
{
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
    int n = sizeof(randArray)/sizeof(randArray[0]);

    oddEvenSort(randArray, n);
    //printArray(randArray, n);

    return (0);
}
